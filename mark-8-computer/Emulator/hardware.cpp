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

static BYTE8 currentKeyboard = 0;

// *******************************************************************************************************************************
//												Byte read from the I/O ports
// *******************************************************************************************************************************

BYTE8 	HWIReadPort(BYTE8 portID,BYTE8 acc) { 
	BYTE8 retVal = 0;
	switch(portID) {
		case 0x00:
			retVal = currentKeyboard;
			break;
	}
	return retVal; 
}

// *******************************************************************************************************************************
//												Byte written to the I/O ports
// *******************************************************************************************************************************

void HWIWritePort(BYTE8 portID,BYTE8 data) {
	switch(portID) {
		case 0x08:																	// $08 is the Display
			IFWriteHogensonDisplay(data);break;
	}

}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	BYTE8 newKeyboard = IFGetKeyboard();											// Any new keyboard depression ?
	if (newKeyboard != 0) {															// Received a new keystroke ?
		currentKeyboard = newKeyboard;												// That is available this time round
		CPURequestInterrupt();														// Cause an interrupt
	} 
}

