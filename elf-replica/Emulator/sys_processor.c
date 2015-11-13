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
#include "sys_debug_system.h"

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CRYSTAL_CLOCK 	(1789773L)													// Clock cycles per second (1.79Mhz)
#define CYCLE_RATE 		(CRYSTAL_CLOCK/8)											// Cycles per second (8 clocks per cycle)
#define FRAME_RATE		(60)														// Frames per second (60)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (3,728)

//	3,668 Cycles per frame
// 	262 lines per video frame
//	14 cycles per scanline (should be :))

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8 ramMemory[MEMORYSIZE];													// Otherwise, use 1k memory

// *******************************************************************************************************************************
//											 Memory and I/O read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															// Too complex to macroize.
#define WRITE() 	_Write()
#define READ16() 	_Read()
#define INPORT() 	MB = 0
#define OUTPORT() 	{}
#define FETCH() 	MA = R[P];READ();ADDRP(1)
#define UPDATEQ()	{}
#define READEFLAG() MB = 1

#include "__1802support.h"

static inline void _Read(void) {
	MB = (MA < MEMORYSIZE) ? ramMemory[MA] : DEFAULT_BUS_VALUE; 					// Reading RAM (0000 up)
}

static void _Write(void) {
	if (MA < MEMORYSIZE) ramMemory[MA] = MB;
}


// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	CPU1802Reset();
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {

	FETCH();opcode = MB;															// Read the opcode
	pReg = &(R[opcode & 0x0F]);														// Set up the current register pointer.

	switch(opcode) {																// Execute it.
		#include "__1802opcodes.h"
	}
	if (cycles >= CYCLES_PER_FRAME-29) {											// If we are at INT time.
		if (IE != 0) {																// and interrupts are enabled
			CPU1802Interrupt();														// Fire an interrupt
			cycles = CYCLES_PER_FRAME - 29;											// Make it EXACTLY 29 cycles to display start
																					// When breaks on FRAME_RATE then will be at render
		}
	}	
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	return FRAME_RATE;																// Return frame rate.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (R[P] != breakPoint1 && R[P] != breakPoint2);							// Stop on breakpoint.
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPUReadMemory(R[P]);												// Current opcode.
	if (opcode >= 0xD0 && opcode <= 0xDF) return (R[P]+1) & 0xFFFF;					// If SEP Rx then step is one after.
	return 0;																		// Do a normal single step
}

// *******************************************************************************************************************************
//												Read/Write Memory
// *******************************************************************************************************************************

BYTE8 CPUReadMemory(WORD16 address) {
	BYTE8 _MB = MB;WORD16 _MA = MA;BYTE8 result;
	MA = address;READ();result = MB;
	MB = _MB;MA = _MA;
	return result;
}

void CPUWriteMemory(WORD16 address,BYTE8 data) {
	BYTE8 _MB = MB;WORD16 _MA = MA;
	MA = address;MB = data;WRITE();
	MB = _MB;MA = _MA;
}

#endif

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.d = D;s.df = DF;s.p = P;s.x = X;s.t = T;s.q = Q;s.ie = IE;					// Registers
	for (int i = 0;i < 16;i++) s.r[i] = R[i];										// 16 bit Registers
	s.cycles = cycles;s.pc = R[P];													// Cycles and "PC"
	return &s;
}

#endif
