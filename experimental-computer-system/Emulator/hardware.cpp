// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (Scelbi specific)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "interface.h"

static BYTE8 keyboardInterruptEnabled = -1;											// Non-zero if keyboard interrupt

// *******************************************************************************************************************************
//												Byte read from the I/O ports
// *******************************************************************************************************************************

BYTE8 	HWIReadPort(BYTE8 portID,BYTE8 acc) { 
	BYTE8 retVal = 0;
	switch(portID) {
		case 0x00:																	// $00 disables interrupt
			keyboardInterruptEnabled = (acc & 1) != 0;break;						// if bit 0 of A clear.
		case 0x01:
			retVal = IFRead(IFR_KEYBOARD);break;									// $01 reads keyboard.
	}
	return retVal; 
}

// *******************************************************************************************************************************
//												Byte written to the I/O ports
// *******************************************************************************************************************************

void HWIWritePort(BYTE8 portID,BYTE8 data) {
	switch(portID) {
		case 0x08:																	// $08 is the Display
			IFWrite(IFW_HOGENSON,data);break;
		case 0x18:																	// $18 is the left lamps
			IFWrite(IFW_LEFTDISPLAY,data);break;
		case 0x19:																	// $19 is the right lamps
			IFWrite(IFW_RIGHTDISPLAY,data);break;
	}

}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	if (keyboardInterruptEnabled != 0) {
		if (IFRead(IFR_INTERRUPTQUERY) != 0) {
			CPURequestInterrupt();
		}
	}
}

// *******************************************************************************************************************************
//												Write to Hogenson Display
// *******************************************************************************************************************************

static BYTE8 xHog,yHog; 													// Position in display
static BYTE8 isHogensonOn = 0; 												// True if scan is on 

void HWIWriteHogensonDisplay(BYTE8 b) {
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
				WRITEDISPLAY(xHog,yHog,(cmd & 1) == 0);						// Set the pixel 2x2,2x4 clear 2x3 2x5
				if (cmd >= 4) {												// Advance, 2x4 2x5
					xHog++;													// Advance across
					if (xHog == 64) yHog++;									// Then down, e.g. clocking
					xHog &= 0x3F;yHog &= 0x3F;								// Put x,y in range 0-63
				}
			}
			break;
	}
}	