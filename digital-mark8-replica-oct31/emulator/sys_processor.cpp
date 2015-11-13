// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Processor Emulation.
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS
#include <stdio.h>
#endif
#ifdef ARDUINO
#include <avr/pgmspace.h>
#endif

#include "sys_processor.h"

static void CPULoadImage(void);

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(500000/2)												// Clock at 500Khz, 2 Clocks per T-State	
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//														Main Memory/CPU
// *******************************************************************************************************************************

static BYTE8 A,B,C,D,E,H,L,pszValue,carry,HaltFlag,MB,PCIX;
static WORD16 temp16,MA,PCR[8],Cycles;

static BYTE8 ramMemory[RAMSIZE];													// RAM memory

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************
//
//	Memory: (see DGPacket #1)
//
//		0000-1DFF 	Low-RAM 	(7,872 bytes max)
//		1EC0-!EFF 	Boot ROM 	(64 x 8 boot ROM, 2 x 8223)
//		2000-3FFF	ROM Space	(8,192 bytes max)

#define READ() 		MB = (MA < RAMSIZE) ? ramMemory[MA] : DEFAULT_BUS_VALUE			// Macros 
#define WRITE() 	if (MA < RAMSIZE) ramMemory[MA] = MB

// *******************************************************************************************************************************
//														I/O Port connections
// *******************************************************************************************************************************

#define INPORT00() 	MB = HWIReadKeyboard()											// Port 0  (Input 0) is the ASCII keyboard
#define INPORT01() 	MB = HWIReadTapeBit() 											// Port 1  (Input 1) is the tape in bit
#define OUTPORT08() HWIWriteTapeBit(MB & 1)											// Port 8  (Output 0) is the tape out bit
#define OUTPORT0E()	HWIWriteVideoPort(MB)											// Port 14 (Output 6) is the Suding TVT

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
	PCIX = 0;PCR[PCIX] = 0;Cycles = 0;
	CPULoadImage();
	HWIReset();
}


// *******************************************************************************************************************************
//													 Execute a single phase.
// *******************************************************************************************************************************

#include "8008/__8008_ports.h"														// Hoover up any undefined ports.

BYTE8 CPUExecuteInstruction(void) {
	if (HaltFlag == 0) {															// CPU is running (not halt)
		FETCH();																	// Fetch and execute
		switch(MB) {																// Do the selected opcode and phase.
			#include "8008/__8008_opcodes.h"
		}
	}	
	if (HaltFlag == 0 && Cycles < CYCLES_PER_FRAME) return 0;						// Frame in progress, return 0.
	if (HaltFlag != 0) Cycles = 0;													// Fix up for HALT.
	Cycles -= CYCLES_PER_FRAME;														// Adjust cycle counter
	HWIEndFrame();																	// Hardware stuff.
	return FRAME_RATE;																// Return the frame rate for sync speed.
}

// *******************************************************************************************************************************
//											Built in image
// *******************************************************************************************************************************

#ifdef WINDOWS
static const BYTE8 __image[] = {
	#include "__image.h"
};
#endif

#ifdef ARDUINO
static const PROGMEM uint16_t __image[] = {
	#include "__image.h"
};
#endif

// *******************************************************************************************************************************
//										  Load image into RAM
// *******************************************************************************************************************************

static void CPULoadImage(void) {
	for (WORD16 n = 0;n < 2048;n++) {
		#ifdef WINDOWS
		ramMemory[n] = __image[n];
		#endif
		#ifdef ARDUINO
		ramMemory[n] = pgm_read_byte_far(__image+n);
		#endif
	}
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
	fread(ramMemory,RAMSIZE,1,f);
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
