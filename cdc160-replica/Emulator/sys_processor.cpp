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
#include "iomanager.h"

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CYCLE_RATE 		((WORD16)(1000000.0/6.4))									// Cycles per second (6.4us cycle time)
#define FRAME_RATE		(25)														// Frames per second (25)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (6,250)

// *******************************************************************************************************************************
//													 Memory & I/O Macros
// *******************************************************************************************************************************

static void inline _Read(void);
static void inline _Write(void);

#define READ() 		_Read()															// These are now functions, may be inlined.
#define WRITE() 	_Write()

#define IOTRANSFER(d) HWMTransferWord(d,&Z);										// Transfer data to/from Z.
#define IOEXTERNAL() HWMExternalFunction(Z);										// Call external function.

// *******************************************************************************************************************************
//												   Generated support code
// *******************************************************************************************************************************

#include "__CDC160Support.h"

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8 _memory[6144];															// 6144 x 8 bytes == 4096 x 12 bytes
																					// First 4k is the lower 8 bits.
																					// Second 2k is alternate high/low (high first)

//	It is done this way because it is planned to run it on a Mega2560 Arduino with 8k of SRAM. Some of this is used by
//	Arduino, some by other code, so we compress 4k x 12 into 6k x 8. In a modern PC the CDC160 is so slow by comparison
//	it doesn't matter, it spends its time just waiting.

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************


static void inline _Read() {
	Z = _memory[(S >> 1)+0x1000];													// These are the upper bits.
	Z = (Z << ((S & 1) ? 8 : 4)) & 0xF00;											// Shift lower or upper into bits 8..11
	Z = Z | _memory[S];																// Or in the lower byte part.
}

static void inline _Write() {
	_memory[S] = (Z & 0xFF);														// Put the lower byte part out.
	if (S & 1) {																	// Odd words, they use the lower part.
		_memory[(S >> 1)+0x1000] = (_memory[(S >> 1)+0x1000] & 0xF0) | (Z >> 8);
	} else {																		// Even words, they use the upper part.
		_memory[(S >> 1)+0x1000] = (_memory[(S >> 1)+0x1000] & 0x0F) | ((Z >> 4) & 0xF0);
	}
}

// *******************************************************************************************************************************
//														CPU Reset
// *******************************************************************************************************************************

void CPUReset(void) {
	CDC160Reset();																	// Reset the CPU
	HWMReset();																		// Reset the hardware
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {
	if (halt != 0) {																// Halted
		cycles = 0;																	// Complete the frame.
		return FRAME_RATE;
	}
	S = P;READ();																	// Read opcode
	switch ((Z >> 6) & 077) {														// Calculate the function code F and switch.
		#include "__CDC160Opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	return FRAME_RATE;																// Return frame rate.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

#include <stdio.h>

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (P != breakPoint1 && P != breakPoint2);									// Stop on breakpoint.
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

WORD16 CPURead(WORD16 address) { 
	WORD16 oldS = S,oldZ = Z,result;												// Save S,Z
	S = address;																	// Read memory
	READ();
	result = Z;
	S = oldS;Z = oldZ;																// Restore S,Z
	return result;
}
void CPUWrite(WORD16 address,WORD16 data) {
	WORD16 oldS = S,oldZ = Z;														// Save S,Z
	S = address;Z = data;															// Write memory
	WRITE();
	S = oldS;Z = oldZ;																// Restore S,Z
}

// *******************************************************************************************************************************
//											Read one line of paper tape
// *******************************************************************************************************************************


int _CPUReadPaperTape(FILE *f) {
	char szBuffer[17],*p = szBuffer;
	fgets(szBuffer,16,f);															// Read one line
	int word = 0;
	while (*p != '\0') {															// Scan it, converting punches
		if (*p == 'O' || *p == '.') {
			word = (word << 1) | (*p == 'O' ? 1 : 0);
		}
		p++;
	}
	return word;
}

// *******************************************************************************************************************************
//												Read tape into computer
// *******************************************************************************************************************************

void CPULoadTape(const char *tapeName,int tapeFormat,int address) {
	S = address;
	FILE *f = fopen(tapeName,"r");													// Open paper tape
	int endOfTape = 0;													
	while (endOfTape == 0) {														// Keep going until done
		int word1 = _CPUReadPaperTape(f);											// Read first half word
		int word2 = _CPUReadPaperTape(f);											// Read second half word
		if ((word1 & 0100) == 0) {													// If no 7th hole on first half, end
			endOfTape = 1;
		} else {
			Z = ((word1 & 077) << 6) | (word2 & 077);								// Construct data
			WRITE();																// Write
			S = (S + 1) & 07777;													// Bump S
		}
	}
	fclose(f);
	S = 0;Z = 07001;WRITE();														// Write JMP @1 in 0
	S = 1;Z = address;WRITE();														// Start of tape in 1
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.p = P;s.a = A;s.z = Z;s.cycles = cycles;s.halt = (halt != 0);
	return &s;
}
#endif
