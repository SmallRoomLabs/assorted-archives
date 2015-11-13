// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.c
//		Purpose:	Processor Emulation.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS
#include <stdio.h>
#endif

#include "sys_processor.h"
#include "sys_debug_system.h"
#include "driver_common.h"

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CYCLE_RATE 		(960000)													// Cycles per second (0.96Mhz)
#define FRAME_RATE		(60)														// Frames per second (50 arbitrary)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (20,000)

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************


static BYTE8 a,x,y,s;																// 6502 A,X,Y and Stack registers
static BYTE8 carryFlag,interruptDisableFlag,breakFlag,								// Values representing status reg
									decimalFlag,overflowFlag,sValue,zValue;
static WORD16 pc;																	// Program Counter.
static BYTE8 ramMemory[RAMSIZE];													// Memory at $0000 upwards
static WORD16 cycles;																// Cycle Count.

#ifdef WINDOWS
#define CONSTTYPE static BYTE8 const 												// Constant for Windows (ROM etc)
#endif
#ifdef ARDUINO
#include <avr/pgmspace.h>															// Constant for Arduino (ROM etc)
#define CONSTTYPE static BYTE8 const PROGMEM
#endif

// *******************************************************************************************************************************
//														ROM Images
// *******************************************************************************************************************************

CONSTTYPE wozniakROM[256] = {														// Woz Monitor (FF00-FFFF)
	#include "code/_code_wozrom.h"
};

CONSTTYPE wozniakTapeROM[256] = {													// Woz Cassette (C100-C1FF)
	#include "code/_code_woztape.h"
};

CONSTTYPE basicROM[4096] = {														// Apple 1 Basic (E000-EFFF)
	#include "code/_code_basic.h"
};

// *******************************************************************************************************************************
//											 Memory and I/O read and write macros.
// *******************************************************************************************************************************

#define READ(a) 	_Read(a)														// Basic Read
#define WRITE(a,d)	_Write(a,d)														// Basic Write

#define SETPC(a) 	pc = (a)														// PC Changed
#define CYCLES(n) 	cycles += (n)													// Bump Cycles
#define FETCH() 	_Read(pc++)														// Fetch byte
#define FETCHWORD()	{ temp16 = FETCH();temp16 |= (FETCH() << 8); }					// Fetch word

#define READWORD(a) (READ(a) | ((READ((a)+1) << 8)))								// Read 16 bit, Basic

//	These are optimised for page 0 and page 1
#define READWORD01(a) ramMemory[a]+(ramMemory[(a)+1] << 8)							// Read 16 bit, page 0/1 
#define READ01(a) 	ramMemory[a]													// Read 8 bit, page 0/1
#define WRITE01(a,d) ramMemory[a] = d												// Write 8 bit, page 0/1

static inline BYTE8 _Read(WORD16 address);											// Need to be forward defined as 
static inline void _Write(WORD16 address,BYTE8 data);								// used in support functions.

#include "6502/__6502support.h"

// *******************************************************************************************************************************
//											   Read and Write Inline Functions
// *******************************************************************************************************************************

static BYTE8 currentKeyPending = 0;

static BYTE8 _ReadHardware(WORD16 address);
static void _WriteHardware(WORD16 address,BYTE8 data);

static inline BYTE8 _Read(WORD16 address) {
	if (address < RAMSIZE) return ramMemory[address];								// RAM from $0000 upwards
	if ((address >> 12) == 0x0E) {													// BASIC ROM from $E000-$EFFF
		#ifdef WINDOWS
		return basicROM[address & 0xFFF];
		#else
		return pgm_read_byte_far(basicROM+(address & 0xFFF));
		#endif
	}
	if (address >= 0xFF00) {														// Monitor ROM from $FF00-$FFFF
		#ifdef WINDOWS
		return wozniakROM[address & 0xFF];
		#else
		return pgm_read_byte_far(wozniakROM+(address & 0xFF));
		#endif
	}
	if ((address >> 8) == 0xC1) {													// Cassette ROM from $C100-$C1FF
		#ifdef WINDOWS
		return wozniakTapeROM[address & 0xFF];
		#else
		return pgm_read_byte_far(wozniakTapeROM+(address & 0xFF));
		#endif		
	}
	return _ReadHardware(address);
}

static inline void _Write(WORD16 address,BYTE8 data) {
	if (address < RAMSIZE) {														// RAM from $0000 upwards
		ramMemory[address] = data;
		return;
	}
	_WriteHardware(address,data);
}

// *******************************************************************************************************************************
//												Read and Write Hardware options
// *******************************************************************************************************************************

static BYTE8 _ReadHardware(WORD16 address) {
	if (address == 0xD010 || address == 0xD011) {									// Keyboard read
		BYTE8 newKey = 0;
		if (newKey == 0) newKey = DRVRead(DRA1_KEYBOARD,0);							// Check the keyboard.

		if (newKey == 0 && currentKeyPending == 0) {								// If no key and no pending
			newKey = IOReadInputStream();											// Grab one from the stream 
			if (newKey == 10) newKey = 13;											// Fix LF, some editors LF only.
		}
		if (newKey != 0x00) {														// Found one, it is "pending"
			if (newKey >= 'a' && newKey <= 'z') newKey -= 0x20;						// Upper case alpha lock (see note pp1)
			if (newKey == 8) newKey = AKEY_BACKSPACE;								// Map 8 (BS) to Apple 2 Backspace
			if (newKey < ' ' && newKey != 0x0D && newKey != 0x1B) newKey = 0;		// Filter out unwanted control (e.g. only ESC and CR)
			currentKeyPending = newKey;								
		}
		if (address == 0xD010) {													// Reading new key.
			BYTE8 r = currentKeyPending | 0x80;										// Return with bit 7 set.
			currentKeyPending = 0;													// But no key pending now.
			return r;
		}
		return (currentKeyPending != 0) ? 0x80 : 0x00;								// Return bit 7 set if key available.
	}
	if (address == 0xD012) return 0x00; 											// Read DSP - display *always* ready.
	return DEFAULT_BUS_VALUE;
}

static void _WriteHardware(WORD16 address,BYTE8 data) {
	if (address == 0xD012) {														// Writing to DSP register ?
		data &= 0x7F;
		if (data >= 'a' && data <= 'z') data = data - 0x20;							// Lower case as upper
		if (data >= 0x20 && data < 0x5F) {											// Displayable characters
			DRVWrite(DWA1_WRITE,data);
		} else {																	// Control characters
			if (data == AKEY_BACKSPACE) DRVWrite(DWA1_BACKSPACE,0);
			if (data == 0x0D) DRVWrite(DWA1_NEWLINE,0);	
		}
	}
}

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

CONSTTYPE prompt[] = "\n*** APPLE 1 REPLICA ***\n(C) PAUL ROBSON 2015\n(PAUL@ROBSONS.ORG.UK)\n\bK MEMORY\n\n";

void CPUReset(void) {
	DRVWrite(DWA1_RESET,0);DRVRead(DRA1_RESET,0);
	resetProcessor();
	BYTE8 n = 0,c;
	do {
		#ifdef WINDOWS
		c = prompt[n];
		#else
		c = pgm_read_byte_far(prompt+n);
		#endif
		if (c == '\n') DRVWrite(DWA1_NEWLINE,0);
		if (c == '\b') {
			BYTE8 k = RAMSIZE/1024;
			if (k >= 10) DRVWrite(DWA1_WRITE,k/10+'0');
			DRVWrite(DWA1_WRITE,k%10+'0');
		}
		if (c >= ' ') DRVWrite(DWA1_WRITE,c);
		n++;
	} while (c != 0);
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

static BYTE8 lastReset = 0;

BYTE8 CPUExecuteInstruction(void) {

	BYTE8 opcode = FETCH();															// Fetch opcode.

	switch(opcode) {																// Execute it.
		#include "6502/__6502opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	BYTE8 newReset = DRVRead(DRA1_ISRESET,0);										// Get new state of reset.
	if (lastReset != 0 && newReset == 0) CPUReset();								// Reset when let go.
	lastReset = newReset;															// Update state.
	DRVWrite(DWA1_FRAME,FRAME_RATE);DRVRead(DRA1_FRAME,FRAME_RATE);					// Mention frame.
	return IOIsStreamLoading() != 0 ? -1 : FRAME_RATE;																// Return frame rate.
}


// *******************************************************************************************************************************
//												Get time in ms (windows only)
// *******************************************************************************************************************************

#ifdef WINDOWS
#include "gfx.h"
LONG32 SYSMilliseconds(void) {
	return GFXTimer();
}
#endif

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (pc != breakPoint1 && pc != breakPoint2);								// Stop on breakpoint.
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPUReadMemory(pc);												// Current opcode.
	if (opcode == 0x20) return (pc+3) & 0xFFFF;										// Step over JSR.
	return 0;																		// Do a normal single step
}

// *******************************************************************************************************************************
//												Read/Write Memory
// *******************************************************************************************************************************

BYTE8 CPUReadMemory(WORD16 address) {
	return READ(address);
}

void CPUWriteMemory(WORD16 address,BYTE8 data) {
	WRITE(address,data);
}

#endif

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT

static CPUSTATUS st;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	st.a = a;st.x = x;st.y = y;st.sp = s;st.pc = pc;
	st.carry = carryFlag;st.interruptDisable = interruptDisableFlag;st.zero = (zValue == 0);
	st.decimal = decimalFlag;st.brk = breakFlag;st.overflow = overflowFlag;
	st.sign = (sValue & 0x80) != 0;st.status = constructFlagRegister();
	st.cycles = cycles;
	return &st;
}

#endif
