// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (8008 Machine specific)
//		Created:	1st November 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"

static BYTE8 keyboardPort;
static BYTE8 pendingKeyPress;
static BYTE8 dirtyLines[64];
static BYTE8 screenNeedsRepaint = 0;
static BYTE8 isInitialised = 0;

// ST7920
// 	VSS,PSB,BLK 	GND	(PSB is mode select, low = SPI)
//	VDD,BLA 		5V
//	RS 				A0 (5V)
//	R/W 			A1 (MOSI)
//	E 				A2 (SCLK)
//
#ifdef ARDUINO
#include <Arduino.h>
#include <PS2Keyboard.h>
#include "ST7920LCDDriver.h"

#define ST7920_RW  			(A1)												// Connections to ST7920
#define ST7920_E  			(A2)
#define ST7920_RS  			(A0)

#define PS2KBD_DATA 		(2)													// These can move, but are suitable
#define PS2KBD_CLOCK 		(3)													// for most things
#define PIEZO_BUZZER 		(12)

static ST7920LCDDriver lcd(ST7920_E, ST7920_RW, 0);
static PS2Keyboard keyboard;
#endif

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	keyboardPort = 0;
	pendingKeyPress = 0;
	for (BYTE8 n = 0;n < 64;n++) HWIMarkDirtyLine(n);
	if (isInitialised == 0) {
		#ifdef ARDUINO
		keyboard.begin(PS2KBD_DATA,PS2KBD_CLOCK);									// Set up PS2
		isInitialised = 1;
		pinMode(ST7920_RS, OUTPUT); 
	  	digitalWrite(ST7920_RS, HIGH);
	  	lcd.begin(true);          
	  	lcd.clear();
		#endif
	}
}

// *******************************************************************************************************************************
//													Read the keyboard port
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboardPort(void) {
	return keyboardPort | 0x80;
}

// *******************************************************************************************************************************
//										Mark a line as dirty as in need of repainting
// *******************************************************************************************************************************

void HWIMarkDirtyLine(BYTE8 y) {
	screenNeedsRepaint = 1;
	dirtyLines[y] = 1;
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	#ifdef ARDUINO
	if (screenNeedsRepaint != 0) {
		screenNeedsRepaint = 0;
		for (BYTE8 y = 0;y < 64;y++) {
			if (dirtyLines[y] != 0) {
				dirtyLines[y] = 0;
				lcd.draw(0,y,CPUGetVideoRAM()+y*16,16);
			}
		}
	}
	if (keyboard.available()) {
		pendingKeyPress = keyboard.read();
		if (pendingKeyPress == PS2_BACKSPACE) pendingKeyPress = 8;			
	}
	#endif
	keyboardPort = pendingKeyPress & 0x7F; 	
	if (keyboardPort != 0) CPURequestInterrupt();
	pendingKeyPress = 0;	
}

#ifdef WINDOWS

// *******************************************************************************************************************************
//									Windows Keyboard Handler (links to Emulator Framework)
// *******************************************************************************************************************************

#include <gfx.h>

int HWIKeyboardHandler(int key,int runMode) {
	if (key != 0 && runMode != 0) {
		pendingKeyPress = GFXToASCII(key,1) & 0x7F;
	}
	return key;
}

#endif

// *******************************************************************************************************************************
//													Set the audio pitch
// *******************************************************************************************************************************

void HWISetAudio(BYTE8 audio) {
	if ((audio & 0x80) != 0) {
		WORD16 freq = (audio & 0x7F) * 12;
		#ifdef WINDOWS
		GFXSetFrequency(freq);
		#endif
		#ifdef ARDUINO
		tone(PIEZO_BUZZER,freq);
		#endif
	} else {
		#ifdef WINDOWS
		GFXSetFrequency(0);
		#endif
		#ifdef ARDUINO
		noTone(PIEZO_BUZZER);
		#endif
	}
}
