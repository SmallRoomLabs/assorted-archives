// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware interface
//		Created:	4th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "hardware.h"

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//													Windows Specific Code
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS

#include "gfx.h"

static int toggles = 0;																// State of toggle switches
static int keyPressed = 0;															// Key pressed this frame
static int isToggles = -1;															// Non zero if toggles,zero if octal
static int currentKey = 0;															// Keyboard input

// *******************************************************************************************************************************
//								Handle keystrokes passed through by the debugger loop
// *******************************************************************************************************************************

int  HWIKeyMap(int key,int inRunMode) {

	if (inRunMode == 0) return key;													// Ignore if not in run mode.

	if (key >= '1' && key <= '8' && isToggles != 0) {								// If in toggle keyboard mode
			toggles ^= (0x80 >> (key - '1'));										// Toggle the switch (keys 1-8)
	}

	if (key == GFXKEY_TAB) isToggles = 0;											// TAB pressed, to octal keypad mode.

	if (isToggles == 0) {															// If in octal keyboard mode
		if (key <= 0) toggles = 0;													// Key released
		if (key >= '0' && key <= '7') toggles = 0x01 << (key-'0');					// Key 0-7 pressed
	}

	if (key > 0x00) {																// Save the key press as ASCII.
		keyPressed = GFXToASCII(key,1);
	}
	return key;
}

// *******************************************************************************************************************************
//											Reset hardware
// *******************************************************************************************************************************

void HWIReset(void) {
}

// *******************************************************************************************************************************
//											Write to the LED Display
// *******************************************************************************************************************************

void HWIWriteLEDDisplay(BYTE8 data) {
	DBGXWriteLEDDisplay(data);
}

// *******************************************************************************************************************************
//											Write to a video memory pixel
// *******************************************************************************************************************************

void HWIWriteVideoMemory(BYTE8 x,BYTE8 y,BYTE8 isOn) {
	DBGXWriteVDUMemory(x,y,isOn);
}

// *******************************************************************************************************************************
//								Read the keyboard. This returns the positive going strobe.
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboard(void) {
	return currentKey;
}

// *******************************************************************************************************************************
//									Read the value on the toggle switches / octal keypad
// *******************************************************************************************************************************

BYTE8 HWIReadToggles(void) {
	return toggles;
}

// *******************************************************************************************************************************
//											Read the control keys A,B and Reset
// *******************************************************************************************************************************

BYTE8 HWIReadControlKey(BYTE8 key) {
	BYTE8 retVal = 0;
	switch(key) {
		case KEY_A:	retVal = GFXIsKeyPressed(GFXKEY_F10);break;
		case KEY_B:	retVal = GFXIsKeyPressed(GFXKEY_F11);break;
		case KEY_RESET:	retVal = GFXIsKeyPressed(GFXKEY_F12);break;
	}
	return retVal;
}

// *******************************************************************************************************************************
//													Set audio frequency
// *******************************************************************************************************************************

void HWISetFrequency(WORD16 freq) {
	GFXSetFrequency(freq);
}

// *******************************************************************************************************************************
//											Update the current keyboard key at end of frame
// *******************************************************************************************************************************

void HWIEndFrameUpdate(void) {

	if (keyPressed != 0) {													// Have we pressed a key
		currentKey = keyPressed | 0x80; 									// Return that key with the positive strobe.
	} else {
		currentKey = 0x00;													// No key pressed this time, reset the strobe
	}
	keyPressed = 0x00;														// Reset current key flag
}

#endif

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//													Arduino Specific Code
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef ARDUINO

#include "lcd7920.h"
#include <PS2Keyboard.h>

static int keyPressed = 0;															// Key pressed this frame
static int currentKey = 0;															// Keyboard input
static int n = 0;
static int currentLED = 1;

static WP1Driver lcd(SCLKpin, MOSIpin, 0);
static PS2Keyboard keyboard;

// *******************************************************************************************************************************
//											Reset hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	//Serial.begin(9600);
	pinMode(SSpin, OUTPUT);   
    digitalWrite(SSpin, HIGH);
    lcd.begin();  		       
	keyboard.begin(DataPin, IRQpin);
	HWIWriteLEDDisplay(0x00);
}

// *******************************************************************************************************************************
//											Write to the LED Display
// *******************************************************************************************************************************

void HWIWriteLEDDisplay(BYTE8 data) {
	if (data != currentLED) {
		lcd.writeHexDigit(5,16,data >> 4);
		lcd.writeHexDigit(6,16,data & 0x0F);
		lcd.writeBinary(4,40,data);
		currentLED = data;
	}
}

// *******************************************************************************************************************************
//											Write to a video memory pixel
// *******************************************************************************************************************************

void HWIWriteVideoMemory(BYTE8 x,BYTE8 y,BYTE8 isOn) {
	lcd.setPixel(x,y,isOn);
}

// *******************************************************************************************************************************
//								Read the keyboard. This returns the positive going strobe.
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboard(void) {
	return currentKey;
}

// *******************************************************************************************************************************
//									Read the value on the toggle switches / octal keypad
// *******************************************************************************************************************************

BYTE8 HWIReadToggles(void) {
	return 0;
}

// *******************************************************************************************************************************
//											Read the control keys A,B and Reset
// *******************************************************************************************************************************

BYTE8 HWIReadControlKey(BYTE8 key) {
	BYTE8 retVal = 0;
	switch(key) {
		case KEY_A:	break;
		case KEY_B:	break;
		case KEY_RESET:	break;
	}
	return retVal;
}

// *******************************************************************************************************************************
//													Set audio frequency
// *******************************************************************************************************************************

void HWISetFrequency(WORD16 freq) {
	if (freq == 0) noTone(TONEPIN);
	else tone(TONEPIN,freq);
}

// *******************************************************************************************************************************
//											Update the current keyboard key at end of frame
// *******************************************************************************************************************************

void HWIEndFrameUpdate(void) {
	lcd.repaint();
	keyPressed = 0;
	if (keyboard.available()) {
		keyPressed = keyboard.read();
	}
	if (keyPressed != 0) {													// Have we pressed a key
		currentKey = keyPressed | 0x80; 									// Return that key with the positive strobe.
	} else {
		currentKey = 0x00;													// No key pressed this time, reset the strobe
	}
	keyPressed = 0x00;														// Reset current key flag
}

#endif
