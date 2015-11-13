// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		xd_keyboard_arduino.cpp
//		Purpose:	External Drivers : PS/2 Keyboard
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <Arduino.h>
#include <PS2Keyboard.h>
#include "drivers.h"

#define DATA_PIN 		(2)													// Connection to PS2 keyboard
#define CLOCK_PIN 		(3)													

static PS2Keyboard keyboard;												// Keyboard object

// *******************************************************************************************************************************
//													Initialise Keyboard Driver
// *******************************************************************************************************************************

void XDKeyboardInit(void) {
	keyboard.begin(DATA_PIN,CLOCK_PIN);										// Set up PS2 keyboard
}

// *******************************************************************************************************************************
//													Return new key press, if any
// *******************************************************************************************************************************

BYTE8 XDKeyboardGetNewKey(void) {
	BYTE8 rv = 0;
	if (keyboard.available()) {												// Key available ?
		rv = keyboard.read();												// Read it.
		if (rv >= 0x80) rv = 0;												// Remove high characters
		if (rv == PS2_BACKSPACE) rv = 8;									// Backspace returns chr(8)
	}
	return rv;
}

