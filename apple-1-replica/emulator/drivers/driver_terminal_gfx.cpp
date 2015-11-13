// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_terminal_gfx.cpp
//		Purpose:	Terminal Driver (GFX)
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "driver_terminal.h"

// *******************************************************************************************************************************
//													Basic Terminal for Debug Window
// *******************************************************************************************************************************

BYTE8 TRMCommand(BYTE8 command,BYTE8 p1,BYTE8 p2,BYTE8 p3) {
	BYTE8 retVal = 0;
	switch(command) {
		case TRC_GETWIDTH:
			retVal = 40;
			break;
		case TRC_GETHEIGHT:
			retVal = 24;
			break;
		case TRC_WRITE:											// Does nothing, updated in Debugger Display
			break;
		case TRC_CLEAR:											// Clears when rendered in debugger display.
			break;

	}
	return retVal;
}
