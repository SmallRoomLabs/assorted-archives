// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	21st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _SYS_PROCESSOR_H
#define _SYS_PROCESSOR_H

typedef unsigned short WORD16;														// 8,16 and 32 bit types.
typedef unsigned int   LONG32;
typedef unsigned char  BYTE8;

#define RAMSIZE (8192)																

#define DEFAULT_BUS_VALUE (0777777)													// What's on the bus if it's not memory.

void CPUReset(void);																// CPU methods
BYTE8 CPUExecuteSinglePhase(void);													// Run one phase of CPU emulation.

WORD16 CPUGetStepOverBreakpoint(void);												// Get step over breakpoint
BYTE8 CPUExecute(WORD16 break1,WORD16 break2);										// Run to break point(s)
BYTE8 CPUExecuteInstruction(void);													// Execute one instruction (multi phases)
void CPUWrite(WORD16 address,LONG32 data);											// Write one word.
void CPUSetPC(WORD16 address);														// Set PC

#define TTY_DISPWIDTH	(19)														// Height and Width of teletype
#define TTY_DISPHEIGHT	(7)															// display

#include "devices.h"

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct _CPUSTATUS {
	int ac,mbr,lr;		
	int tac,tsbr;	
	int halt;										
	int pc;																			
	int cycles;
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);														// Access CPU State
LONG32 CPURead(WORD16 address);														// Access RAM

#endif
#endif