// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.h
//		Purpose:	Processor Emulation (header)
//		Created:	1st November 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _SYS_PROCESSOR_H
#define _SYS_PROCESSOR_H

typedef unsigned short WORD16;														// 8 and 16 bit types.
typedef unsigned char  BYTE8;
typedef unsigned int   LONG32;

#define ROMSIZE 	(8192)															// Allocated ROM.
#define RAMSIZE 	(4096)															// 4k RAM we have.

#define DEFAULT_BUS_VALUE (0xFF)													// What's on the bus if it's not memory.
#define PCTR	PCR[PCIX]

void CPUReset(void);																// CPU methods
BYTE8 CPUExecuteInstruction(void);													// Execute one instruction (multi phases)
void CPURequestInterrupt(void); 													// Request interrupt.
BYTE8 *CPUGetVideoRAM(void);

void HWIReset(void);																// Reset hardware.
void HWIEndFrame(void);																// End frame.
BYTE8 HWIReadKeyboardPort(void);
void HWISetAudio(BYTE8 audio);
void HWIMarkDirtyLine(BYTE8 y);

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