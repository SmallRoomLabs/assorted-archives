// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Processor Emulation.
//		Created:	1st November 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS
#include <stdio.h>
#define ROMCODE 	static BYTE8
#endif

#ifdef ARDUINO
#include <Arduino.h>
#include <avr/pgmspace.h>
#define ROMCODE static const BYTE8 PROGMEM 
#endif

#include "sys_processor.h"

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(500000/2)												// Clock at 800Khz, 2 Clocks per T-State	
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//														Main Memory/CPU
// *******************************************************************************************************************************

static BYTE8 A,B,C,D,E,H,L,pszValue,carry,HaltFlag,MB,PCIX;							// Registers, flags, temporary values.
static WORD16 temp16,MA,PCR[8],Cycles,interruptRequested;

ROMCODE romMemory[ROMSIZE] = {														// ROM Memory (0000-1FFF)
	#include "__image.h"
};

static BYTE8 ramMemory[RAMSIZE];													// RAM memory (2000 upwards)
static BYTE8 videoMemory[1024];														// VRAM memory (3C00-3FFF)

// *******************************************************************************************************************************
//														Memory Read/Write
// *******************************************************************************************************************************

#define READ() 		_Read()
#define WRITE() 	_Write()

static void inline _Read(void) {
	if (MA < 0x2000) {
		#ifdef WINDOWS
		MB = romMemory[MA];
		#endif
		#ifdef ARDUINO
		MB = pgm_read_byte_far(romMemory+MA);
		#endif
		return;
	}
	if (MA < 0x2000+RAMSIZE) {
		MB = ramMemory[MA & 0x1FFF];
		return;
	}
	if (MA >= 0x3C00) {
		MB = videoMemory[MA & 0x3FF];
	}
	MB = DEFAULT_BUS_VALUE;
}

static void inline _Write(void) {
	if (MA >= 0x2000 && MA < 0x2000+RAMSIZE) {
		ramMemory[MA & 0x1FFF] = MB;
	}
	if (MA >= 0x3C00) {
		WORD16 vMA = MA & 0x3FF;
		if (videoMemory[vMA] != MB) {
			videoMemory[vMA] = MB;
			HWIMarkDirtyLine(vMA >> 4);
		}
	}
}

// *******************************************************************************************************************************
//														I/O Port connections
// *******************************************************************************************************************************

#define INPORT00()		MB = HWIReadKeyboardPort();
#define OUTPORT08() 	HWISetAudio(MB);

// *******************************************************************************************************************************
//													Support Macros and Functions
// *******************************************************************************************************************************

#define CYCLES(n)		Cycles += (n)
#define FETCH()			{ MA = PCTR;READ();PCTR = (PCTR + 1) & 07777; }
#define FETCHWORD()		{ MA = PCTR;READ();temp16 = MB;MA++;READ();MA = (MB << 8) | temp16; PCTR = (PCTR + 2) & 07777; }
#define JUMP()			PCTR = MA
#define CALL()			{ PCIX = (PCIX + 1) & 7;PCTR = MA; }
#define RETURN()		PCIX = (PCIX - 1) & 7;

static void _BinaryAdd(void) {
	temp16 = A + MB + carry;														// Calc result
	carry = temp16 >> 8;															// Get new carry
	MB = temp16 & 0xFF;																// Result in MB
}

static void _BinarySubtract(void){	
	temp16 = A - MB - carry;														// Calc result
	carry = (temp16 >> 8) & 1;														// Get new borrow
	MB = temp16 & 0xFF;																// Result in MB
}

static BYTE8 _IsParityEven(BYTE8 n) { 
	BYTE8 isEven = -1;																// 0 is even parity
	while (n != 0) {																// until all bits test
		if (n & 1) isEven = !isEven;												// each set bit toggles it
		n = n >> 1;
	}
	return isEven; 																
}

// *******************************************************************************************************************************
//													  Port interfaces
// *******************************************************************************************************************************

void CPUReset(void) {
	A = B = C = D = E = H = L = pszValue = carry = HaltFlag = 0;
	PCIX = 0;PCR[PCIX] = 0;Cycles = 0;interruptRequested = 0;
	HWIReset();
}


// *******************************************************************************************************************************
//													 Execute a single phase.
// *******************************************************************************************************************************

#include "8008/__8008_ports.h"														// Hoover up any undefined ports.

BYTE8 CPUExecuteInstruction(void) {
	if (interruptRequested != 0) {													// Interrupt requested (keystroke)
		interruptRequested = 0;														// Clear flag and HALT
		HaltFlag = 0;																// But we don't do anything.
	}
	if (HaltFlag == 0) {															// CPU is running (not halt)
		FETCH();																	// Fetch and execute
		switch(MB) {																// Do the selected opcode and phase.
			#include "8008/__8008_opcodes.h"
		}
	}	
	if (HaltFlag == 0 && Cycles < CYCLES_PER_FRAME) return 0;						// Frame in progress, return 0.
	if (HaltFlag != 0) Cycles = 0;													// Fix up for HALT.
	Cycles -= CYCLES_PER_FRAME;														// Adjust cycle counter
	HWIEndFrame();																	// Hardware update.d
	return FRAME_RATE;																// Return the frame rate for sync speed.
}

// *******************************************************************************************************************************
//													Request an interrupt
// *******************************************************************************************************************************

void CPURequestInterrupt(void) {
	interruptRequested = 1;
}

BYTE8 *CPUGetVideoRAM(void) {
	return videoMemory;
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//										 Get the step over breakpoint value
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPURead(PCTR);												// Read opcode.
	if ((opcode & 0xC7) == 0x07) return ((PCTR+1) & 0x3FFF);						// RST xx
	if ((opcode & 0xC3) == 0x42) return ((PCTR+3) & 0x3FFF);						// CALL xxxx (various calls)
	return 0xFFFF;
}

// *******************************************************************************************************************************
//										Run continuously till breakpoints / Halt.
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 break1,WORD16 break2) {
	BYTE8 rate = 0;
	while(1) {
		rate = CPUExecuteInstruction();												// Execute one instruction phase.
		if (rate != 0) {															// If end of frame, return rate.
			return rate;													
		}
		if (PCTR == break1 || PCTR == break2) return 0;
	} 																				// Until hit a breakpoint or HLT.
}

// *******************************************************************************************************************************
//												Load a binary file into RAM
// *******************************************************************************************************************************

void CPULoadBinary(char *fileName) {
	FILE *f = fopen(fileName,"rb");
	fread(romMemory,RAMSIZE,1,f);
	fclose(f);
}

// *******************************************************************************************************************************
//												Gets a pointer to RAM memory
// *******************************************************************************************************************************
	
BYTE8 CPURead(WORD16 address) {
	WORD16 _MA = MA;BYTE8 _MB = MB;BYTE8 result;
	MA = address;READ();result = MB;
	MA = _MA;MB = _MB;
	return result;
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	int i;
	s.a = A;s.b = B;s.c = C;s.d = D;s.e = E;s.h = H;s.l = L;						// 8 bit registers
	s.zFlag = (pszValue == 0);s.cFlag = (carry != 0);s.hFlag = (HaltFlag != 0);		// Flags
	s.pFlag = (_IsParityEven(pszValue) != 0);s.sFlag = ((pszValue & 0x80) != 0);
	s.cycles = Cycles;																// Number of cycles.
	for (i = 0;i < 8;i++) s.stack[i] = 0;											// Clear stack.
	s.pc = PCTR;																	// Save PC.
	i = PCIX-1;s.stackDepth = 0;													// Copy stack.
	while (i >= 0) s.stack[s.stackDepth++] = PCR[i--];
	s.hl = (s.h << 8) | s.l;s.m = CPURead(s.hl & 0x3FFF);						// Helpers
	return &s;
}
#endif
