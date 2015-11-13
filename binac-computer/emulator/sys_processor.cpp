// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Binac Processor Emulation.
//		Created:	12th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#ifdef WINDOWS
#include <stdio.h>
#endif

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(100000)												// 10us Clock Speed (actually its 4Mhz)
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//														Main Memory, CPU
// *******************************************************************************************************************************

static LONG32 ACC;																	// 31 bit accumulator
static LONG32 L;																	// 31 bit L register
static WORD16 PC;																	// 9+1 bit program counter
static BYTE8  HALT;																	// Halt flag
static WORD16 Cycles;

static LONG32 memory[MEMSIZE];														// Program/Data Memory
	
// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#define READ(a)		memory[(a) & MEMMASK]
#define WRITE(a,d) 	memory[(a) & MEMMASK] = (d)

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

#define RWRITE(m,a,b) 	memory[m] |= ((((a) << 9) | b) << 15)
#define LWRITE(m,a,b) 	memory[m] |= ((((a) << 9) | b) << 0)

void CPUReset(void) {
	PC = 0;HALT = 0;																// Clear PC and HALT.


	RWRITE(0,011,000);		
	LWRITE(0,010,010);		
	RWRITE(1,003,010);

	ACC = 01234567;
	WRITE(010,01000000000);
	L = 041010;
}

// *******************************************************************************************************************************
//													 Execute a single instruction.
// *******************************************************************************************************************************

#define CYCLES(n) 	Cycles += ((n+5)/10)

BYTE8 CPUExecuteInstruction(void) {
	if (HALT != 0) return FRAME_RATE;												// CPU is Halted

	int opcode = READ(PC >> 1);														// Read opcode, 2 per word
	if ((PC & 1) == 0) opcode = opcode >> 15;										// Work out correct half.
	PC = (PC + 1) & (MEMMASK * 2 + 1);												// Bump PC

	int order = (opcode >> 9) & 037;												// Get Order
	opcode = opcode & 0777;															// Get Opcode

	switch(order) {
		case 001:	CYCLES(123);													// 01 STOP : Stop Computer
					HALT = 1;
					break;
		case 002:	CYCLES(123);													// 02 F/ADDL : Add L to A
					ACC = BINMAdd(ACC,L);
					break;
		case 003:	CYCLES(633);													// 03 D/DIV : Divide A by (m)
					ACC = BINMDivide(ACC,READ(opcode));
					L = (ACC ^ 0xFACEEBDF) & MASK31;								// Put some junk in L, unknown state.
					break;
		case 004:	CYCLES(285);													// 04 C/STC : Store A in (m) and Clear A
					WRITE(opcode,ACC);
					ACC = 0;
					break;
		case 005:	CYCLES(285);													// 05 A/ADD : Add (m) to A
					ACC = BINMAdd(ACC,READ(opcode));
					break;
		case 010:	CYCLES(654);													// 10 M/MUL : Multiply L by (m) to A
					ACC = BINMMultiply(L,READ(opcode));
					break;
		case 011:	CYCLES(123);													// 11 K/TALC : Transfer A to L clear A
					L = ACC;ACC = 0;
					break;
		case 012:	CYCLES(123);													// 11 L/TAL : Transfer A to L
					L = ACC;
					break;
		case 013:	CYCLES(285);													// 13 H/STA : Store A in (m) 
					WRITE(opcode,ACC);
					break;
		case 014:	CYCLES(123);													// 14 T/JMC : Jump if -ve to m, clear A always
					if ((ACC & SIGN31) != 0) PC = opcode * 2; 
					ACC = 0;
					break;
		case 015:	CYCLES(285);													// 15 S/SYV : Subtract (m) from A
					ACC = BINMSubtract(ACC,READ(opcode));
					break;
		case 020:	CYCLES(123);													// 20 U/JMP : Jump to address
					PC = opcode * 2;
					break;
		case 022:	CYCLES(123);													// 22 +/ASHL : Shift left
					ACC = (ACC << 1) & MASK31; 
					break;
		case 023:	CYCLES(123);													// 23 +/ASHR : Arithmetic Shift right
					ACC = ((ACC >> 1) | (ACC & SIGN31)) & MASK31; 
					break;
		case 024:	CYCLES(123);													// 24 BP/BRK : Never stops.
					break;
		case 025:	CYCLES(123);													// 25 SKIP/NOP: No operation/skip
					break;
	}

	if (HALT == 0 && Cycles < CYCLES_PER_FRAME) return 0;							// Frame in progress, return 0.
	Cycles = Cycles - CYCLES_PER_FRAME;
	if (HALT != 0) Cycles = 0;														// Fix up for HALT.
	return FRAME_RATE;																// Return the frame rate for sync speed.
}

// *******************************************************************************************************************************
//												Core Memory Read/Write
// *******************************************************************************************************************************
	
LONG32 CPURead(WORD16 address) {
	return READ(address);
}

void CPUWrite(WORD16 address,LONG32 data) {
	WRITE(address,data);
}


#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//										 Get the step over breakpoint value
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
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

#include <stdio.h>

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.acc = ACC;
	s.l = L;
	s.halt = HALT;
	s.mpc = PC >> 1;
	s.mpcHalf = PC & 1;
	s.cycles = Cycles;
	return &s;
}
#endif
	