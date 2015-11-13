// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (C8008 specific)
//		Created:	22nd October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS
#include <stdio.h>
#include "gfx.h"
#endif

#ifdef ARDUINO
#include <Arduino.h>
#include <PS2Keyboard.h>
#include "ST7920LCDDriver.h"
#endif

#include "sys_processor.h"

static BYTE8 videoRAM[128];															// 16 x 8 Video RAM.
static BYTE8 dirtyFlag[64]; 														// 64 dirty flags
static BYTE8 needsRepaint = 0;														// Non-zero when needs repainting.
static BYTE8 pendingKey = 0;														// Key ready ?

#ifdef ARDUINO
#define SPI_MOSI   		A1
#define SPI_CLK   		A2
#define SPI_ENABLE     	A0 
static ST7920LCDDriver lcd(SPI_CLK, SPI_MOSI, 0);

#define DATA_PIN 		(2)															// Connection to PS2 keyboard
#define CLOCK_PIN 		(3)													
static PS2Keyboard keyboard;														// Keyboard object

#endif


// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
#ifdef ARDUINO
	pinMode(SPI_ENABLE, OUTPUT);   
  	digitalWrite(SPI_ENABLE, HIGH);
  	lcd.begin(true);          
  	lcd.clear();
  	for (BYTE8 n = 0;n < 128;n++) videoRAM[n] = ' ';								// VRAM mirrors.
	keyboard.begin(DATA_PIN,CLOCK_PIN);												// Set up PS2 keyboard
#endif
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
#ifdef ARDUINO
	if (needsRepaint) {																// Needs repainting.
		needsRepaint = 0;															// Clear flag
		for (BYTE8 n = 0;n < 64;n++) {												// Scan all flags
			if (dirtyFlag[n] != 0) {													// Dirty flag set ?
				dirtyFlag[n] = 0;													// Clear it
				lcd.drawText(n % 8,n / 8 * 8,videoRAM+n*2);							// Update display.
			}
		}
	}
	if (keyboard.available()) {														// Key available ?
		pendingKey = keyboard.read();												// Read it.
		if (pendingKey >= 0x80) pendingKey = 0;										// Remove high characters
		if (pendingKey == PS2_BACKSPACE) pendingKey = 8;							// Backspace returns chr(8)
	}
#endif
}

// *******************************************************************************************************************************
//													Write to video memory
// *******************************************************************************************************************************

void HWIWriteVideoMemory(BYTE8 address,BYTE8 character) {
	address &= 0x7F;																	// 128 bytes VRAM
	if (videoRAM[address] != character) {												// Changed ?
		videoRAM[address] = character;													// Change in VRAM
		dirtyFlag[address >> 1] = 1;													// Set dirty flag
		needsRepaint = 1;
	}
}

// *******************************************************************************************************************************
//													  Access video memory
// *******************************************************************************************************************************

BYTE8 *HWIGetVideoMemory(void) {
	return videoRAM;
}


// *******************************************************************************************************************************
//									Debugger key intercept handler
// *******************************************************************************************************************************

#ifdef WINDOWS
int HWIProcessKey(int key,int isRunTime) {
	if (key != 0 && isRunTime != 0) {										// Running and key press
		BYTE8 newKey = GFXToASCII(key,1);									// Convert to ASCII
		if (newKey != 0) pendingKey = newKey;								// Put pending key in buffer
	}
	return key;
}
#endif

// *******************************************************************************************************************************
//											Read keyboard
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboard(void) {
	BYTE8 rv = 0;
	if (pendingKey != 0) {													// Key waiting.
		rv = pendingKey | 0x80;												// Return it with bit 7 set
		pendingKey = 0;														// Clear buffer
	}
	return rv;
}
