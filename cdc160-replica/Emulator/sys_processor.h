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

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct __CPUSTATUS {
	int p,a,z,cycles,halt;															// Returned values.
} CPUSTATUS;

void CPUReset(void);
BYTE8 CPUExecuteInstruction(void);
CPUSTATUS *CPUGetStatus(void);
WORD16 CPURead(WORD16 address);
void CPUWrite(WORD16 address,WORD16 data);
BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2);
WORD16 CPUGetStepOverBreakpoint(void);
void CPULoadTape(const char *tapeName,int tapeFormat,int startAddress);
#endif

#endif