// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_gfxin.c
//		Purpose:	Input Driver, utilising GFX system. Required for standard Windows Build.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include "gfx.h"
#include "driver_common.h"

static BYTE8 keyAvailable = 0;

BYTE8 DRVGFXHandler(BYTE8 key,BYTE8 isRunMode) {
	if (key != 0 && isRunMode) {
		BYTE8 asc = GFXToASCII(key,1);
		keyAvailable = asc;
	}
	return key;
}

// *******************************************************************************************************************************
//												GFX-Linked Input
// *******************************************************************************************************************************

DRVPARAM DRVRead(BYTE8 command,DRVPARAM data) {
	DRVPARAM retVal = 0x00;
	switch(command) {
		case DRA1_KEYBOARD:
			if (keyAvailable != 0) {												// Keystroke available
				retVal = keyAvailable;												// Return as ASCII.
				if (retVal == 0x09) retVal = 0x1B;									// Map TAB onto ESCape
				keyAvailable = 0;													// Only send it once !
			}
			break;
		case DRA1_ISRESET:
			retVal = GFXIsKeyPressed(GFXKEY_DOWN);
			break;

	}
	return retVal;
}
