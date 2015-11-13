// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		xd_keyboard_windows.cpp
//		Purpose:	External Drivers : Windows Keyboard
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "drivers.h"
#include "gfx.h"

static  BYTE8 pressedKey;

// *******************************************************************************************************************************
//													Initialise Keyboard Driver
// *******************************************************************************************************************************

void XDKeyboardInit(void) {
	pressedKey = 0;
}

// *******************************************************************************************************************************
//													Return new key press, if any
// *******************************************************************************************************************************

BYTE8 XDKeyboardGetNewKey(void) {
	BYTE8 rv = pressedKey;
	pressedKey = 0;
	return rv;
}

// *******************************************************************************************************************************
//													Debugger key intercept handler
// *******************************************************************************************************************************

int HWIProcessKey(int key,int isRunTime) {
	if (key != 0 && isRunTime != 0) {										// Running and key press
		BYTE8 newKey = GFXToASCII(key,1);									// Convert to ASCII
		if (newKey != 0) pressedKey = newKey;								// Save for passsing back to hardware interface
	}
	return key;
}

