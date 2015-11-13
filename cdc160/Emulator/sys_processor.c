// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.c
//		Purpose:	CDC160 Processor Emulation.
//		Created:	2nd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "sys_debug_system.h"

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CYCLE_RATE 		(156250)													// Cycles per second (6.4us cycle time)
#define FRAME_RATE		(25)														// Frames per second (25)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (6,250)

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static WORD16 P;																	// P register (program counter)
static WORD16 A;																	// A register (accumulator)
static WORD16 Z;																	// Z register (internal)
static WORD16 S;																	// Storage location register (internal)
static WORD16 B;																	// Auxiliary register (internal)
static WORD16 cycles; 																// Cycle counter
static BYTE8  haltFlag;																// Non-zero when halted.

#ifdef ARDUINO																		// Memory in Processor.
static BYTE8 compressedMemory[6144];												// Allocate 6k of RAM in 2560 for 4k x 12
																					// 8k will not fit :)
#else
static WORD16 memory[4096];															// Otherwise, use 4096 x 16 bit words.
#endif

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#ifdef ARDUINO																		// Compressed R/W
// TODO: Arduino read/write functionality into a 6k x 8 byte block.
#else																				// Simple R/W
#define READ() 		Z = memory[S]													// Read. 											
#define WRITE() 	memory[S] = Z 													// Write.
#endif

// *******************************************************************************************************************************
//															Other macros
// *******************************************************************************************************************************

#define CYCLES(n)			cycles += n 											// Bump cycle counter.
#define ADD(n) 				B = n;CPUAddToAccumulator()								// Add (n) to A via B
#define SHIFTINSTRUCTION()	CPUShiftInstruction()									// Handle Shift instructions.
#define ROTATE1(w) 			(((w << 1) || (w >> 11)) & 07777)						// One position rotate left.
#define BLOCKTRANSFER(d) 	CPUBlockTransfer(d)										// INP OUT block transfer instructions.

#define IOTRANSFER(d)		{}														// Transfer data to/from Z.
#define IOFUNCTIONCODE(n)	{}														// Transmit function code
#define IOERROR() 			(0)														// Non zero when IO error has occurred on transfer.

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	P = A = Z = 0;																	// Zero the main registers.
	cycles = 0;																		// Clear cycles
	haltFlag = 0;																	// Not halted.
	S = B = 0;																		// Clear S,B as well.

/*
	memory[0] = 05410;			// AOD 10											// Speed throttle test code.
	memory[1] = 06501;			// NZB 01
	memory[2] = 05411;			// AOD 11
	memory[3] = 06503;			// NZB 03
	memory[4] = 05412;			// AOD 12
	memory[5] = 06505;			// NZB 05
	memory[6] = 07700;			// HLT
*/
}

// *******************************************************************************************************************************
//											Add B to accumulator, 12 bit one's complement
// *******************************************************************************************************************************

static void CPUAddToAccumulator(void) {
	if (B == 07777 && A == 07777) return; 											// -0 + -0 == -0 (special case)
	A = A + B;																		// Add B to A
	if ((A & 010000) != 0) A = (A & 07777) + 1;										// End around carry.
	if (A == 07777) A = 0;															// Zero fix.
}

// *******************************************************************************************************************************
//															Implement SHA
// *******************************************************************************************************************************

static void CPUShiftInstruction(void) {
	switch(Z & 077) {
		case 002:																	// 0102 Shift left once.
			A = ROTATE1(A);
			break;
		case 010:																	// 0110 Shift left three times.
			A = ROTATE1(A);A = ROTATE1(A);A = ROTATE1(A);
			break;
		case 012:																	// 0112 Multiply by 10.
			Z = ROTATE1(A);															// Z = A * 2
			A = ROTATE1(Z);A = ROTATE1(A);											// A = Z * 4 = A * 8
			ADD(Z);																	// A = A * 8 + A * 2 = A * 10
			break;
	}
}

// *******************************************************************************************************************************
//														I/O Block Transfer
// *******************************************************************************************************************************

static void CPUBlockTransfer(char direction) {
	S = (P + (Z & 077)) & 07777;													// 1) P + E -> S - now points to start address
	READ();A = Z;																	// 2) Read start address into A
	P = (P + 1) & 07777;															// 3) Increment P by one, terminating address
	do {
		if (direction == 'I') {														// 4) (Both)
			IOTRANSFER('I');S = A;WRITE();											// 4a) Data read from i/p device, stored at addr A
		} else {
			S = A;READ();IOTRANSFER('O');											// 4b) Obtain data from address A and send to o/p
		}
		A = (A + 1) & 07777;														// 5) Increase A by one.
		S = P;READ();																// 6) Contents compared with term. address at P.
	} while (IOERROR() == 0 && Z != A);												// 7) If P != A go back to step 4 (exit on error)
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {
	if (haltFlag != 0) {															// Halted
		cycles = 0;																	// Complete the frame.
		return FRAME_RATE;
	}
	S = P;READ();																	// Read opcode
	switch ((Z >> 6) & 077) {														// Calculate the function code F and switch.
		#include "__opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	return FRAME_RATE;																// Return frame rate.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1) { 
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (P != breakPoint1);														// Stop on breakpoint.
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	return 0;																		// Do a normal single step
}

// *******************************************************************************************************************************
//												Read/Write external.
// *******************************************************************************************************************************

WORD16 CPURead(WORD16 address) { return memory[address & 07777]; }
void CPUWrite(WORD16 address,WORD16 data) { memory[address & 07777] = data & 07777; }

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.p = P;s.a = A;s.z = Z;s.cycles = cycles;s.halt = (haltFlag != 0);
	return &s;
}
#endif
