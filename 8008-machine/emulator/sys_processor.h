// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	21st October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _SYS_PROCESSOR_H
#define _SYS_PROCESSOR_H

typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned char  BYTE8;

#ifdef ARDUINO
#define RAMSIZE	(6144)																// On the Arduino Mega, we have 6k of RAM (?)
#else
#define RAMSIZE (8192)																// Otherwise it is all RAM (mapped anyway)
#endif

#define DEFAULT_BUS_VALUE (0xFF)													// What's on the bus if it's not memory.

void CPUReset(void);																// CPU methods
void CPURequestInterrupt(void);														// Request Interrupt
BYTE8 CPUExecuteInstruction(void);													// Execute one instruction (multi phases)

void HWIReset(void);																// Reset hardware.
void HWIEndFrame(void);																// End frame.

#ifdef INCLUDE_DEBUGGING_SUPPORT													// Only required for debugging

typedef struct _CPUSTATUS {
	int a,b,c,d,e,h,l,zFlag,pFlag,cFlag,sFlag,hFlag;								// 8008 registers
	int pc,stack[8],stackDepth;														// PC and Stack.
	int cycles;																		// Elapsed cycles
	int hl,m;																		// Helper stuff.
} CPUSTATUS;

CPUSTATUS *CPUGetStatus(void);														// Access CPU State
void CPULoadBinary(char *fileName);													// Load Binary in.
BYTE8 CPURead(WORD16 address);														// Access RAM
BYTE8 CPUExecute(WORD16 break1,WORD16 break2);										// Run to break point(s)
WORD16 CPUGetStepOverBreakpoint(void);												// Get step over breakpoint

#endif
#endif