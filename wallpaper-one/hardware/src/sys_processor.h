// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	2nd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _PROCESSOR_H
#define _PROCESSOR_H

typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned char  BYTE8;

#define DEFAULT_BUS_VALUE (0xFF)													// What's on the bus if it's not memory.

void CPUReset(void);
BYTE8 CPUExecuteInstruction(void);

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct __CPUSTATUS {
	int a,e,s,p0,p1,p2,p3,cycles;
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);
BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2);
WORD16 CPUGetStepOverBreakpoint(void);
BYTE8 CPUReadMemory(WORD16 address);
void CPUWriteMemory(WORD16 address,BYTE8 data);

#endif
#endif