// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.cpp
//		Purpose:	Processor Emulation.
//		Created:	29th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdlib.h>
#ifdef WINDOWS
#include <stdio.h>
#endif
#include "sys_processor.h"
#include "sys_debug_system.h"
#include "hardware.h"

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
//														ROM Images
// *******************************************************************************************************************************

#define ROMIMAGE static BYTE8 const
ROMIMAGE monitorROM[] = {
	#include "binaries/monitor_rom.h"
};

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8   	D,DF,X,P,Q,T,IE,opcode,MB;
static WORD16   R[16],MA,LBA,cycles;

static BYTE8 ramMemory[MEMORYSIZE];													

// *******************************************************************************************************************************
//											 Memory and I/O read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															
#define WRITE() 	_Write()
#define READ16() 	_Read();LBA=MB;MA++;_Read();LBA = (LBA << 8)|MB
#define FETCH() 	MA = R[P];READ();ADDRP(1)

static inline void _Read(void) {
	if (MA < MEMORYSIZE) {
		MB = ramMemory[MA];
		return;
	}
	if (MA >= 0x8000 && MA < 0x8200) {
		MB = monitorROM[MA-0x8000];
		return;
	}
	MB = DEFAULT_BUS_VALUE;
}

static inline void _Write(void) {
	if (MA < MEMORYSIZE) ramMemory[MA] = MB;
}

// *******************************************************************************************************************************
//													   Port Interfaces
// *******************************************************************************************************************************

#define INPORT1() 	(HWISetScreenOn(1),DEFAULT_BUS_VALUE)							// INP 1 screen on
#define OUTPORT1(n)	HWISetScreenOn(0)												// OUT 1 screen off

#define EFLAG1() 	(1)																// EF1 is always set.
#define EFLAG3() 	(1) 															// Is key pressed ?

#include "1802/__1802ports.h"														// Default connections.
#include "1802/__1802support.h"

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	HWIReset();
	CPU1802Reset();
	R[2] = 0x8008;																	// LDI 80 ; PHI R2 ; LDI 08 ; PLO R2
	D = 8;																			// As a result of previous instruction.
	X = 2; 	P = 2; 																	// SEX R2 ; SEP R2
	R[0] = 0x8008;																	// R0 will have advance to 8008.
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {

	FETCH();opcode = MB;															// Read the opcode
	pReg = &(R[opcode & 0x0F]);														// Set up the current register pointer.

	switch(opcode) {																// Execute it.
		#include "1802/__1802opcodes.h"
	}

	if (cycles >= CYCLES_PER_FRAME-29) {											// If we are at INT time.
		if (IE != 0) {																// and interrupts are enabled
			CPU1802Interrupt();														// Fire an interrupt
			cycles = CYCLES_PER_FRAME - 29;											// Make it EXACTLY 29 cycles to display start
																					// When breaks on FRAME_RATE then will be at render
		}
	}	
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	BYTE8 *ptr = NULL;																// NULL if R[0] is a bad pointer.							
	if (R[0] <= MEMORYSIZE-256) ptr = ramMemory+R[0];								// If in memory range, get pointer
	HWISetPageAddress(R[0],ptr);													// Set the display address.
	HWIEndFrame();																	// End of Frame code
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

// *******************************************************************************************************************************
//												Load a binary file into RAM
// *******************************************************************************************************************************

#include <stdio.h>

void CPULoadBinary(const char *fileName) {
	FILE *f = fopen(fileName,"rb");
	fread(ramMemory,1,MEMORYSIZE,f);
	fclose(f);
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.d = D;s.df = DF;s.p = P;s.x = X;s.t = T;s.q = Q;s.ie = IE;					// Registers
	for (int i = 0;i < 16;i++) s.r[i] = R[i];										// 16 bit Registers
	s.cycles = cycles;s.pc = R[P];													// Cycles and "PC"
	return &s;
}

#endif
