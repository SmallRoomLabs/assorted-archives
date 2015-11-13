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

static BYTE8 keyboardPort;														// Keyboard port value exc. bit 7
static BYTE8 pendingKeyPress;													// Any key press that happened this frame
static BYTE8 dirtyLines[64];													// Screen lines need repainting
static BYTE8 screenNeedsRepaint = 0;											// Flagged when any needs repainting
static BYTE8 isInitialised = 0;

// ST7920 SPI GLCD 128x64 Wiring.
//
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

#define PS2KBD_DATA 		(2)													// Connections to PS/2 Keyboard Adaptor
#define PS2KBD_CLOCK 		(3)													
#define PIEZO_BUZZER 		(12)												// Connections to buzzer.

static ST7920LCDDriver lcd(ST7920_E, ST7920_RW, 0);
static PS2Keyboard keyboard;
#endif

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	keyboardPort = 0;															// Initialise states
	pendingKeyPress = 0;
	for (BYTE8 n = 0;n < 64;n++) HWIMarkDirtyLine(n);
	if (isInitialised == 0) {
		#ifdef ARDUINO
		keyboard.begin(PS2KBD_DATA,PS2KBD_CLOCK);								// Set up PS2 keyboard i/f
		isInitialised = 1;
		pinMode(ST7920_RS, OUTPUT); 											// Set up GLCD
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
	return keyboardPort | 0x80;													// Return with DB7 set
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
	if (screenNeedsRepaint != 0) {												// If any line needs repainting
		screenNeedsRepaint = 0;													// Clear flag
		for (BYTE8 y = 0;y < 64;y++) {											// Look for dirty lines
			if (dirtyLines[y] != 0) {
				dirtyLines[y] = 0;
				lcd.draw(0,y,CPUGetVideoRAM()+y*16,16);							// If found, repaint them
			}
		}
	}
	if (keyboard.available()) {													// PS2 keyboard char ?
		pendingKeyPress = keyboard.read();										// Save it
		if (pendingKeyPress == PS2_BACKSPACE) pendingKeyPress = 8;				// Convert Backspace to CHR(8)
	}
	#endif
	keyboardPort = pendingKeyPress & 0x7F; 										// Save any new key
	if (keyboardPort != 0) CPURequestInterrupt();								// Fire int on any new key
	pendingKeyPress = 0;														// Clear the key press buffer
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
		WORD16 freq = (audio & 0x7F) * 12;										// Calc tone
		#ifdef WINDOWS 															// Set sound for Arduino/Windows
		GFXSetFrequency(freq);
		#endif
		#ifdef ARDUINO
		tone(PIEZO_BUZZER,freq);
		#endif
	} else {																	// Sound off.
		#ifdef WINDOWS
		GFXSetFrequency(0);
		#endif
		#ifdef ARDUINO
		noTone(PIEZO_BUZZER);
		#endif
	}
}
