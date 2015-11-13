// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		debug_system.h
//		Purpose:	Debugger Code (System Dependent) Header
//		Created:	4th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"

#ifndef _DEBUG_SYS_H
#define _DEBUG_SYS_H
#include "sys_processor.h"

#define WIN_TITLE 		"CDC160 Emulator"											// Initial Window stuff
#define WIN_WIDTH		1280
#define WIN_HEIGHT		768
#define WIN_BACKCOLOUR	0x004

// *******************************************************************************************************************************
//							These functions need to be implemented by the dependent debugger.
// *******************************************************************************************************************************

#define DEBUG_ARGUMENTS(ac,av) CPULoadTape("code.bioctal",TAPE_BIOCTAL,0100)		// Handle CLI arguments

#define TAPE_BIOCTAL 		1

#define DEBUG_CPURENDER(x) 	DBGXRender(x)											// Render the debugging display
#define DEBUG_VDURENDER(x)	DBGXRender(x)											// Render the game display etc.

#define DEBUG_RESET() 		CPUReset()												// Reset the CPU / Hardware.
#define DEBUG_HOMEPC()		(CPUGetStatus()->p) 									// Get PC Home Address (e.g. current PCTR value)

#define DEBUG_SINGLESTEP()	CPUExecuteInstruction()									// Execute a single instruction, return 0 or Frame rate on frame end.
#define DEBUG_RUN(b1,b2) 	CPUExecute(b1,b2)										// Run a frame or to breakpoint, returns -1 if breakpoint
#define DEBUG_GETOVERBREAK() CPUGetStepOverBreakpoint()								// Where would we break to step over here. (0 == single step)

#define DEBUG_RAMSTART 		(0)														// Initial RAM address for debugger.
#define DEBUG_SHIFT(d,v)	((v < 8) ? ((((d) << 3) | v) & 07777) : d)				// Shifting into displayed address.

#define DEBUG_KEYMAP(k,r)	(k)

void DBGXRender(int *address);														// Render the debugger screen.
void DBGXWriteTTYDisplay(BYTE8 character);											// Write to teletype visual device in debug.

#endif