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
#include "drivers.h"

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	DRVInitialise();
}

// *******************************************************************************************************************************
//							Read strobe. To make it work, we clear the strobe on the first read.
// *******************************************************************************************************************************

static BYTE8 currentKeyPortValue = 0;

BYTE8 HWIReadKeyboard(void) {
	BYTE8 retVal = 0;
	if (currentKeyPortValue != 0) {													// Received a key in the last end frame.
		retVal = currentKeyPortValue | 0x80;										// Return it with the strobe bit set
		currentKeyPortValue = 0;
	}
	return retVal;																	// Return the key.
}
// *******************************************************************************************************************************
//													Write to video port
// *******************************************************************************************************************************

static BYTE8 lastPortValue = 0;														// Last value written to port
static BYTE8 vduAddress = 0;														// Address in TVT clocks

void HWIWriteVideoPort(BYTE8 data) {

	if ((lastPortValue & 0x80) != 0 && (data & 0x80) == 0) {						// Port value goes 1->0
		if (lastPortValue == 0xFF) {												// Writing $FF resets the counter
			vduAddress = 0;
		} else {
			DRVWriteDisplay(vduAddress,lastPortValue);								// Other values go to the display
			vduAddress = (vduAddress + 1) & 0xFF;									// Bump the counter.
		}
	}
	lastPortValue = data;															// Update last written value.
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	BYTE8 newKey = DRVGetNextKey();													// Get any key presses that have taken place.
	if (newKey != 0) currentKeyPortValue = newKey;									// If key pressed put to be read by INPUT.
	DRVEndFrame();																	// Update driver
}
