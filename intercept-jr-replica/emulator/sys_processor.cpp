// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Processor Emulation.
//		Created:	21st October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(2000000)												// Clocks per second	
#define FRAME_RATE			(120)													// Frame rate (120Hz Clock)
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//														Main Memory
// *******************************************************************************************************************************

static WORD16 AC,MQ,PC,temp16,Cycles,MA,MB,IR;
static BYTE8  L,temp8,HALT;
static WORD16 memory[4096];															// 4k Memory Space

#define CYCLES(n)	Cycles += (n)													// Bump cycle counter

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															// Macros are inline.
#define WRITE() 	_Write()

static inline void _Read();															// Forward reference
static inline void _Write();

static inline void _Read() {
	MB = memory[MA];
}

static inline void _Write() {
	memory[MA] = MB;
}

#define SWITCHREGISTER()		0

// *******************************************************************************************************************************
//														System control
// *******************************************************************************************************************************

static BYTE8 interruptRequested = 0;												// When non-zero, interrupt has been requested.

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

#define CMD(c,i,z,a) 	((c) << 9)+(i ? 00400:0)+(z ? 00200: 0)+ ((a) & 00177)

#define AND(i,z,a) CMD(0,i,z,a)
#define	TAD(i,z,a) CMD(1,i,z,a)
#define ISZ(i,z,a) CMD(2,i,z,a)
#define DCA(i,z,a) CMD(3,i,z,a)
#define JMS(i,z,a) CMD(4,i,z,a)
#define JMP(i,z,a) CMD(5,i,z,a)
#define IOT(n) 	   ((n) | 06000)
#define OPR(n) 	   ((n) | 07000)

void CPUReset(void) {
	AC = MQ = L = 0;																// Reset the CPU
	PC = 07777;																		// Start from here.
}

// *******************************************************************************************************************************
//													   Fire interrupt ?
// *******************************************************************************************************************************

void CPURequestInterrupt(void) {
	interruptRequested = 1;															// Request Interrupt
}

// *******************************************************************************************************************************
//										PDP-8 Effective address calculation
// *******************************************************************************************************************************

static void inline _CPUEAC(void) {
	MA = IR & 00177;																// Lower 7 bits.

	if ((IR & 00200) != 0) MA = MA | ((PC-1) & 07600);								// Upper 5 bits if Z flag is '1'.

	if ((IR & 00400) != 0) {														// Is the I flag set.
		if ((MA >> 3) == 1) {														// Is it 0010-0017 ?
			READ();MB = (MB + 1) & 07777;WRITE();									// Bump the memory location.
			MA = MB;																// That read value is the effective address
			CYCLES(6);
		} else {
			READ();MA = MB;															// Otherwise just read it.
			CYCLES(5);
		}
	}
}

// *******************************************************************************************************************************
//													 Execute a single phase.
// *******************************************************************************************************************************

#define SKIP(test) 	if (test) PC = (PC + 1) & 07777									// Skip macro
#define SIGN(n) 	(n & 04000)														// Get sign bit

BYTE8 CPUExecuteInstruction(void) {
	if (HALT == 0) {																// CPU is running (not halt)

		MA = PC;PC = (PC + 1) & 07777;												// Fetch instruction
		READ();IR = MB;

		switch(MB >> 9) {															// Opcode in upper 3 bits

			case 0:																	// 0xxx AND
				_CPUEAC();															// Fetch operand
				READ();
				AC = AC & MB;
				CYCLES(10);
				break;

			case 1:																	// 1xxx TAD
				_CPUEAC();															// Fetch operand
				READ();
				AC = AC + MB;
				if (AC > 07777) {													// Carry out, complement L.
					AC &= 07777;
					L ^= 1;
				}
				CYCLES(10);
				break;

			case 2:																	// 2xxx ISZ
				_CPUEAC();															// EAC
				READ();																// Read
				MB = (MB + 1) & 07777;												// Bump
				WRITE();															// Update
				SKIP(MB == 0);														// Skip if zero
				CYCLES(16);
				break;

			case 3:																	// 3xxx DCA
				_CPUEAC();															// EAC
				MB = AC;WRITE();													// Write out
				AC = 0;																// Clear A
				CYCLES(11);
				break;

			case 4:																	// 4xxx JMS
				_CPUEAC();															// Calc EAC
				MB = PC;WRITE();													// Save return address
				PC = (MA + 1) & 07777;												// Get next instruction from next word
				CYCLES(11);
				break;

			case 5:																	// 5xxx JMP
				_CPUEAC();															// Calc EAC
				PC = MA;															// Get next instruction from there.
				CYCLES(10);
				break;

			case 7:																	// 7xxx OPR
				switch(MB) {
					#include "__im6100_opcodes.h"									// Autogenerated Micro Ops.
					default:														// Unknown, but use cycles up.
						CYCLES(10);break;
				}
				break;
		}
	}	
	if (interruptRequested != 0) {													// Interrupt requested
		interruptRequested = 0;														// We no longer have a request.
		MA = 0;MB = PC;WRITE();														// Write return address to 0
		PC = 1;																		// Transfer to 1 (this is JMS 0)
	}	 
	if (HALT == 0 && Cycles < CYCLES_PER_FRAME) return 0;							// Frame in progress, return 0.
	if (HALT != 0) Cycles = 0;														// Fix up for HALT.
	Cycles -= CYCLES_PER_FRAME;														// Adjust cycle counter
	return FRAME_RATE;																// Return the frame rate for sync speed.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//										 Get the step over breakpoint value
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	WORD16 opcode = CPURead(PC);													// Read opcode.
	if ((opcode >> 9) == 4) return (PC + 1) & 07777;								// Skip over JMS
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
		if (PC == break1 || PC == break2) return 0;
	} 																				// Until hit a breakpoint or HLT.
}

// *******************************************************************************************************************************
//												Load a binary file into RAM
// *******************************************************************************************************************************

#include <stdio.h>

void CPULoadBinary(char *fileName) {
}

// *******************************************************************************************************************************
//												Gets a pointer to RAM memory
// *******************************************************************************************************************************
	
WORD16 CPURead(WORD16 address) {
	WORD16 _MA = MA;WORD16 _MB = MB;WORD16 result;
	MA = address;READ();result = MB;
	MA = _MA;MB = _MB;
	return result;
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.ac = AC;s.l = L;s.mq = MQ;s.pc = PC;s.halt = HALT;s.cycles = Cycles;
	return &s;
}
#endif
