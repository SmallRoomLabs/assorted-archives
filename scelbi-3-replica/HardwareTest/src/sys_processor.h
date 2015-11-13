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

typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned char  BYTE8;

#ifdef ARDUINO
#define RAMSIZE	(6144)																
#else
#define RAMSIZE (8192)																
#endif

#define DEFAULT_BUS_VALUE (0xFF)													// What's on the bus if it's not memory.

void CPUReset(void);																// CPU methods
BYTE8 CPUExecuteSinglePhase(void);													// Run one phase of CPU emulation.

WORD16 CPUGetStepOverBreakpoint(void);												// Get step over breakpoint
BYTE8 CPUExecute(WORD16 break1,WORD16 break2);										// Run to break point(s)
BYTE8 CPUExecuteInstruction(void);													// Execute one instruction (multi phases)

BYTE8 DRVKeyMapper(BYTE8 key,BYTE8 inRunMode);
void DBGXWriteDisplay(BYTE8 x,BYTE8 y,BYTE8 ch);									// Write to display screen.

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct _CPUSTATUS {
	int a,b,c,d,e,h,l,zFlag,pFlag,cFlag,sFlag,hFlag;								// 8008 registers
	int pc,stack[8],stackDepth;														// PC and Stack.
	int status,interruptMode,singleStepMode,cpuPhase;								// Internal stuff.
	int cycles;																		// Elapsed cycles
	int hl,m;																		// Helper stuff.
	int addressLamps,dataLamps;														// The lamp displays.
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);														// Access CPU State
void CPULoadBinary(const char *fileName);											// Load Binary in.
BYTE8 CPURead(WORD16 address);														// Access RAM
void CPURequestInterrupt(void);														// Request Interrupt

#endif
#endif