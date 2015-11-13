// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_arduino_ps2_in.cpp
//		Purpose:	Input Driver, Arduino PS/2 Keyboard Interface.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <Arduino.h>
#include <PS2Keyboard.h>
#include "driver_common.h"

static BYTE8 isInitialised = 0;
static BYTE8 pendingReset = 0;
static BYTE8 pendingKey = 0;

static PS2Keyboard keyboard;

#define DATA_PIN 		(2)													// These can move, but are suitable
#define CLOCK_PIN 		(3)													// for most things.

// *******************************************************************************************************************************
//												 Update keyboard
// *******************************************************************************************************************************

static void _updateKeyboard() {
	if (keyboard.available()) {
		pendingKey = keyboard.read();										// Keyboard
		if (pendingKey == PS2_BACKSPACE) pendingKey = 8;					// Backspace should return 8.
		if (pendingKey == PS2_DOWNARROW) { 									// Down arrow is the Reset button.
			pendingReset = 1;pendingKey = 0; 
		}		
	}
}

// *******************************************************************************************************************************
//												Arduino In from PS/2
// *******************************************************************************************************************************

DRVPARAM DRVRead(BYTE8 command,DRVPARAM data) {
	if (isInitialised == 0) {
		keyboard.begin(DATA_PIN,CLOCK_PIN);									// Set up PS2
		isInitialised = 1;
	}
	DRVPARAM retVal = 0x00;
	switch(command) {
		case DRA1_KEYBOARD:
			_updateKeyboard();												// If we test key here only, reset won't work if keyboard not read
			retVal = pendingKey;
			pendingKey = 0;
			break;

		case DRA1_ISRESET:
			_updateKeyboard();
			retVal = pendingReset;											// If reset been hit, reset the A1.
			pendingReset = 0;
			break;

	}
	return retVal;
}
