// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.c
//		Purpose:	Processor Emulation.
//		Created:	21st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "__tx0_support.h"
#include "devices.h"

#ifdef WINDOWS
#include <stdio.h>
#endif

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(1000000/6)												// One cycle is 6 us.
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//													   Processor status
// *******************************************************************************************************************************

LONG32 	AC;																			// 18 bit accumulator
WORD16 	PCTR; 																		// 16 bit program counter
LONG32 	LR; 																		// 32 bit link register
WORD16 	MAR;																		// 16 bit memory address
LONG32 	MBR;																		// 18 bit memory buffer
BYTE8 	HALT;																		// 1 bit halt flag
WORD16 	Cycles;																		// Cycles counter
LONG32 	Instruction;																// Current instruction.

// *******************************************************************************************************************************
//														Main Memory
// *******************************************************************************************************************************

static LONG32 ramMemory[RAMSIZE];													// System RAM.							

#define READ() 		MBR = ((MAR < RAMSIZE) ? ramMemory[MAR] : DEFAULT_BUS_VALUE);
#define WRITE() 	if (MAR < RAMSIZE) ramMemory[MAR] = MBR;

void CPUWrite(WORD16 address,LONG32 data) {
	MAR = address;MBR = data;WRITE();
}

void CPUSetPC(WORD16 address) {
	PCTR = address;
}

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	AC = PCTR = LR = MAR = MBR = HALT = Cycles = Instruction = 0;
}

#define IOREAD(d) 	(0)
#define IOWRITE(d,n) {}

// *******************************************************************************************************************************
//											Execute a single instruction
// *******************************************************************************************************************************

static LONG32 temp;

BYTE8 CPUExecuteInstruction(void) {

	if (HALT != 0) return FRAME_RATE;												// CPU Stopped.

	MAR = PCTR;																		// PC -> MA
	PCTR = (PCTR + 1) & 0xFFFF;														// Bump PCTR
	READ();																			// Read Memory
	Instruction = MBR;																// Contains instruction
	switch(Instruction >> 16) {
		case 0:																		// 00 STO
			MAR = Instruction & 0xFFFF;												// Put address part in MAR
			MBR = AC;WRITE();														// Write AC there
			break;
		case 1:																		// 01 ADD
			MAR = Instruction & 0xFFFF;												// Put address part in MAR
			READ();																	// Read it
			AC = TX0Add(AC,MBR);													// Add to AC
			break;
		case 2:																		// 02 TRN
			if ((AC & 0400000) != 0) PCTR = Instruction & 0xFFFF;					// Jump if -ve
			break;
		case 3:																		// 03 OPR
			#include "__tx0_operations.h"
			break;
	}
	Cycles = Cycles + 2;															// 2 Cycles per instruction
	if (Cycles < CYCLES_PER_FRAME) return 0;										// Not end of frame.
	Cycles = Cycles - CYCLES_PER_FRAME;												// Decrement
	return FRAME_RATE;
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//										 Get the step over breakpoint value
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	if ((CPURead(PCTR) >> 16) == 2) {												// If on TRN instruction.
		return AC;																	// the return address is in AC.
	}
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
//												Reads RAM Memory
// *******************************************************************************************************************************
	
LONG32 CPURead(WORD16 address) {
	WORD16 _MAR = MAR;LONG32 _MBR = MBR;LONG32 result;
	MAR = address;READ();result = MBR;
	MAR = _MAR;MBR = _MBR;
	return result;
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.pc = PCTR;s.ac = AC;s.lr = LR;s.mbr = MBR;
	s.tac = IOREAD(DV_TOGGLESWACC);
	s.tsbr = IOREAD(DV_TOGGLESWBUF);
	s.halt = HALT;
	s.cycles = Cycles;																// Number of cycles.
	return &s;
}
#endif
