// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	22nd October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _SYS_PROCESSOR_H
#define _SYS_PROCESSOR_H

typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned int   LONG32;
typedef unsigned char  BYTE8;

#define DEFAULT_BUS_VALUE (0xFF)													// What's on the bus if it's not memory.

void CPUReset(void);																// CPU methods
void CPURequestInterrupt(void);														// Request Interrupt
BYTE8 CPUExecuteInstruction(void);													// Execute one instruction (multi phases)

void HWIReset(void);																
void HWIEndFrame(void);																
void HWIWriteVideoMemory(BYTE8 address,BYTE8 character);
BYTE8 *HWIGetVideoMemory(void);
int HWIProcessKey(int key,int isRunTime);
BYTE8 HWIReadKeyboard(void);

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct _CPUSTATUS {
	int a,e,s;
	int p0,p1,p2,p3;
	int cycles;
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);														// Access CPU State
void CPULoadBinary(const char *fileName);											// Load Binary in.
BYTE8 CPUExecute(WORD16 break1,WORD16 break2);										// Run to break point(s)
WORD16 CPUGetStepOverBreakpoint(void);												// Get step over breakpoint
BYTE8 CPUReadMemory(WORD16 address);												// Access RAM and ROM.
void CPUWriteMemory(WORD16 address,BYTE8 data);
#endif
#endif
