// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		8008.H
//		Purpose:	8008 Emulator optimised for size (Header)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _8008_H
#define _8008_H

typedef unsigned char BYTE8;														// 8 bit unsigned type
typedef unsigned short WORD16;														// 16 bit unsigned type

void  CPUReset(void);																// Reset processor
BYTE8 CPUExecute(WORD16 frameSize);													// Execute a single instruction.
void  CPUInterrupt(BYTE8 vector);													// Cause an interrupt.

// *******************************************************************************************************************************
//		This is optional. Unless you are writing something with a debugger, you don't need this code and it will waste space in
//		a microcontroller.
// *******************************************************************************************************************************

#ifdef READS_CPU_STATUS																// Does it actually need this ?
typedef struct _CPU_Status {														// This structure returns the flags etc.
	int A,B,C,D,E,H,L;																// Main 8 bit registers.
	int zFlag,cFlag,nFlag,pFlag,hFlag;												// 4 flag values and Halt.
	int PC,returnStack[7],stackDepth;												// Current PC, Stack and Stack Depth.
	int cycles;																		// Current cycle count in frame.
	int HL,M;																		// Helpful other values.
} CPUSTATUS;
CPUSTATUS *CPUGetStatus(void);														// This function reads it.
#endif

// *******************************************************************************************************************************
//						These functions are defined externally to access memory and I/O ports.
// *******************************************************************************************************************************

BYTE8 CPUXRead(WORD16 address);
void  CPUXWrite(WORD16 address,BYTE8 data);
BYTE8 CPUXInputOutput(BYTE8 portID,BYTE8 acc);


#endif
