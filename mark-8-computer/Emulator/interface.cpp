// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		interface.c
//		Purpose:	Scelbi controls/display interface
//		Created:	10th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "interface.h"
#include <stdio.h>

static void _IFWriteDisplay(BYTE8 x,BYTE8 y,BYTE8 isOn);

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//														WINDOWS Interface
//	
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS

#include "gfx.h"
#include "sys_debug_system.h"

static BYTE8 currentKey = 0;

// *******************************************************************************************************************************
//									Process keys from the debugger in RUN mode
// *******************************************************************************************************************************

int DBGXProcessDebugKey(int pendingKey,int runMode) {
	if (pendingKey != 0) 
		currentKey = GFXToASCII(pendingKey,1);
	return pendingKey;
}

// *******************************************************************************************************************************
//											Reset or Initialise the Interface
// *******************************************************************************************************************************

void IFReset(void) {
	IFWriteHogensonDisplay(0201);											// Turn display off.
}

// *******************************************************************************************************************************
//										Get key in ASCII if any, clears on read.
// *******************************************************************************************************************************

BYTE8 IFGetKeyboard() {
	BYTE8 retVal = currentKey;
	currentKey = 0;
	return retVal;
}

static void _IFWriteDisplay(BYTE8 x,BYTE8 y,BYTE8 isOn) {
	DBGXWriteDisplay(x,y,isOn);
}

#endif

// *******************************************************************************************************************************
//												Write to Hogenson Display
// *******************************************************************************************************************************

static BYTE8 xHog,yHog; 													// Position in display
static BYTE8 isHogensonOn = 0; 												// True if scan is on 

void IFWriteHogensonDisplay(BYTE8 b) {
	BYTE8 cmd;
	switch (b >> 6) {
		case 0:																// 0aa octal, set X
			xHog = b & 077;break;
		case 1:																// 1aa octal, set Y
			yHog = b & 077;break;
		case 2:																// 2xa octal, command
			cmd = b & 7;
			if (cmd == 1 || cmd == 6) isHogensonOn = (cmd == 6);			// Handle 2x1, 2x6, scan off/on.
			if (cmd >= 2 && cmd <= 5) {										// Drawing.
				_IFWriteDisplay(xHog,yHog,(cmd & 1) == 0);					// Set the pixel 2x2,2x4 clear 2x3 2x5
				if (cmd >= 4) {												// Advance, 2x4 2x5
					xHog++;													// Advance across
					if (xHog == 64) yHog++;									// Then down, e.g. clocking
					xHog &= 0x3F;yHog &= 0x3F;								// Put x,y in range 0-63
				}
			}
			break;
	}
}	