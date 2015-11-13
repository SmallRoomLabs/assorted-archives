// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.c
//		Purpose:	Processor Emulation.
//		Created:	2nd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "interface.h"

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(500000/2)												// 2 T States per Clock Cycle	
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// Cycles per frame.

// *******************************************************************************************************************************
//														Main Memory
// *******************************************************************************************************************************

static BYTE8 ramMemory[RAMSIZE];													// 4k RAM

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															// R/W inline code.
#define WRITE() 	_Write()

static inline void _Read();
static inline void _Write();

#include "__8008support.h"

static inline void _Read() {
	MB = (MA < RAMSIZE) ? ramMemory[MA] : DEFAULT_BUS_VALUE;
}

static inline void _Write() {
	if (MA < RAMSIZE) ramMemory[MA] = MB;
}

// *******************************************************************************************************************************
//											Other macros that link to functions.
// *******************************************************************************************************************************

#define READPORT() 	MB = HWIReadPort(MA & 0xFF,A)
#define WRITEPORT()	HWIWritePort(MA & 0xFF,MB)

// *******************************************************************************************************************************
//														System control
// *******************************************************************************************************************************

static BYTE8 interruptRequested = 0;												// When non-zero, interrupt has been requested.

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	CPU8008Reset();																	// Reset the processor.
	IFReset();																		// Reset the hardware interface.
}

// *******************************************************************************************************************************
//													   Fire interrupt ?
// *******************************************************************************************************************************

void CPURequestInterrupt(void) {
	interruptRequested = 1;															// Request Interrupt
}

// *******************************************************************************************************************************
//													 Execute a single phase.
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {
	if (HaltFlag == 0) {															// CPU is running (not halt)
		FETCH();																	// Fetch and execute
		switch(MB) {																// Do the selected opcode and phase.
			#include "__8008opcodes.h"
		}
	}	
	if (interruptRequested != 0) {													// Interrupt requested
		interruptRequested = 0;														// We no longer have a request.
		HaltFlag = 0;																// We are no longer halted.
		// No instruction performed, NOP on switches.
	}	 
	if (HaltFlag == 0 && Cycles < CYCLES_PER_FRAME) return 0;						// Frame in progress, return 0.
	if (HaltFlag != 0) Cycles = 0;													// Fix up for HALT.
	Cycles -= CYCLES_PER_FRAME;														// Adjust cycle counter
	HWIEndFrame();																	// Hardware stuff.
	return FRAME_RATE;																// Return the frame rate for sync speed.
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

#include <stdio.h>

void CPULoadBinary(char *fileName) {
	FILE *f = fopen(fileName,"rb");
	fread(ramMemory,1,RAMSIZE,f);
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
	s.zFlag = (PSZValue == 0);s.cFlag = (Carry != 0);s.hFlag = (HaltFlag != 0);		// Flags
	s.pFlag = (_Parity(PSZValue) != 0);s.sFlag = ((PSZValue & 0x80) != 0);
	s.cycles = Cycles;																// Number of cycles.
	for (i = 0;i < 8;i++) s.stack[i] = 0;											// Clear stack.
	s.pc = PCTR;																	// Save PC.
	i = PCIX-1;s.stackDepth = 0;													// Copy stack.
	while (i >= 0) s.stack[s.stackDepth++] = PCR[i--];
	s.hl = (s.h << 8) | s.l;s.m = CPURead(s.hl & 0x3FFF);						// Helpers
	return &s;
}
#endif
