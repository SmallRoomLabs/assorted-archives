// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		xd_display_arduino_st7920.cpp
//		Purpose:	External Drivers : Arduino Display on 128x64 ST 7920 Parallel GLCD 32x8 3x5 pixel fonts.
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <Arduino.h>
#include "ST7920LCDDriver.h"
#include "drivers.h"

#define DIRTY_GROUPING 	(4)													// Char grouping dirty flags

static BYTE8 dirtyFlag[256/DIRTY_GROUPING];									// Dirty flags
static BYTE8 screenNeedsPaint = 0;											// True if a dirty flag set.

const int MOSIpin = A1; // 11												// Connection to ST7920
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; // 10

static ST7920LCDDriver lcd(SCLKpin, MOSIpin, 0);							// LCD Driver object

// *******************************************************************************************************************************
//												Screen Initialise
// *******************************************************************************************************************************

void XDDisplayInit(void) {
	pinMode(SSpin, OUTPUT);   												// Enable SS pin on SPI
	digitalWrite(SSpin, HIGH);
	lcd.begin(true);          												// Start driver
	lcd.clear();															// Clear screen
	screenNeedsPaint = 0;
}

// *******************************************************************************************************************************
//									Screen has changed, update it or mark it dirty
// *******************************************************************************************************************************

void XDDisplayWrite(BYTE8 address,BYTE8 data) {
	dirtyFlag[address >> 2] = 1;											// Set the dirty flag
	screenNeedsPaint = 1;													// Set the repaint flag.
}

// *******************************************************************************************************************************
//							Repaint screen if using dirty/repaint or backbuffer approach
// *******************************************************************************************************************************

void XDDisplayRepaint(void) {
	if (screenNeedsPaint != 0) {											// Repainting required ?
		BYTE8 *videoRAM = HWIGetVideoMemory();								// Get pointer to 1/4 VRAM
		screenNeedsPaint = 0;												// Clear flag
		for (BYTE8 p = 0;p < 256/DIRTY_GROUPING;p++) {						// Look at each 4-char chunk
			if (dirtyFlag[p]) {												// Is it dirty ?
				dirtyFlag[p] = 0;											// Clear it and repaint it
				lcd.drawText((p*DIRTY_GROUPING % 32) / DIRTY_GROUPING,
							 p*DIRTY_GROUPING / 32 * 8,
							 videoRAM+p*DIRTY_GROUPING);					
			}
		}
	}
}
