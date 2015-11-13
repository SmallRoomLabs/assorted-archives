// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	12th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _SYS_PROCESSOR_H
#define _SYS_PROCESSOR_H

#include "binac_arithmetic.h"

#define MASK31 017777777777
#define SIGN31 010000000000

#define MEMSIZE 			(512) 													// Number of 12 bit words
#define MEMMASK 			(0777)													// Memory address mask.

typedef unsigned int   LONG32;														// 32 bit type
typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned char  BYTE8;

void CPUReset(void);																// CPU methods
BYTE8 CPUExecuteInstruction(void);
LONG32 CPURead(WORD16 address);														// Access RAM
void CPUWrite(WORD16 address,LONG32 data);

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct _CPUSTATUS {
	int acc,l,halt,mpc,mpcHalf,cycles;
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);														// Access CPU State

BYTE8 CPUExecute(WORD16 break1,WORD16 break2);										// Run to break point(s)
WORD16 CPUGetStepOverBreakpoint(void);												// Get step over breakpoint

#endif
#endif