// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (DGM8 specific)
//		Created:	23rd October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS																// Windows specific includes
#include <stdio.h>
#include "gfx.h"
#endif

#ifdef ARDUINO 																// Arduino specific includes
#include <Arduino.h>
#include <PS2Keyboard.h>
#include "ST7920LCDDriver.h"
#endif

#include "sys_processor.h"

#define DIRTY_GROUPING 	(4)													// Char grouping dirty flags

static BYTE8 videoRAM[256];													// 32 x 8 TVT Memory
static BYTE8 dirtyFlag[256/DIRTY_GROUPING];									// Dirty flags
static BYTE8 screenNeedsPaint = 0;											// True if a dirty flag set.
static BYTE8 isInitialised = 0;												// Non zero when initialised
static BYTE8 vRAMPointer = 0;												// offset into video RAM
static BYTE8 videoLast = 0;													// Last value written to TVT
static BYTE8 pendingKey = 0;												// Key ready ?

//	The dirty flag being in character groups of 4 is because the STM7920, the original LCD target,
//	takes 16 bit horizontal words, which with a 3x5 font has 4 characters in it. If using a OLED
//	128x64 this may incur a small time penalty.

#ifdef WINDOWS
static FILE *fTapeOut;														// Tape output file
static FILE *fTapeIn;														// Tape input file
#endif

#ifdef ARDUINO
const int MOSIpin = A1; // 11												// Connection to ST7920
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; // 10

#define DATA_PIN 		(2)													// Connection to PS2 keyboard
#define CLOCK_PIN 		(3)													

static ST7920LCDDriver lcd(SCLKpin, MOSIpin, 0);							// LCD Driver object
static PS2Keyboard keyboard;												// Keyboard object

#endif

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	if (isInitialised == 0) {
		isInitialised = 1;
		for (WORD16 n = 0;n < 256;n++) videoRAM[n] = 0; 					// Clear VRAM (wouldn't happen)
		pendingKey = 0;														// Clear keyboard buffer
		#ifdef WINDOWS
		fTapeOut = fopen("tape.out","wb");									// Open tape output file.
		fTapeIn = fopen("tape.in","rb");
		#endif

		#ifdef ARDUINO
		pinMode(SSpin, OUTPUT);   											// Enable SS pin on SPI
  		digitalWrite(SSpin, HIGH);
  		lcd.begin(true);          											// Start driver
  		lcd.clear();														// Clear screen
		keyboard.begin(DATA_PIN,CLOCK_PIN);									// Set up PS2 keyboard
		#endif
	}
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	#ifdef ARDUINO
	if (screenNeedsPaint != 0) {											// Repainting required ?
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
	if (keyboard.available()) {												// Key available ?
		pendingKey = keyboard.read();										// Read it.
		if (pendingKey >= 0x80) pendingKey = 0;								// Remove high characters
		if (pendingKey == PS2_BACKSPACE) pendingKey = 8;					// Backspace returns chr(8)
	}
	#endif
}

// *******************************************************************************************************************************
//														Read video memory 
// *******************************************************************************************************************************

BYTE8 HWIReadVideoMemory(BYTE8 offset) {
	return videoRAM[offset];
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

// Implemented so the strobe vanishes immediately on reading. While not technically correct, the
// coder cannot know at IN 0 where it is in the strobe, it *might* finish in the next cycle. Coders
// could read the keyboard and then re-read it to get the key again, but shouldn't !

// *******************************************************************************************************************************
//									Write byte to video memory port
// *******************************************************************************************************************************

void HWIWriteVideoPort(BYTE8 n) {
	if ((n & 0x80) != 0 && (videoLast & 0x80) == 0) {						// Low->High transition on 7
		if (n == 0xFF) {													// 0xFF home cursor
			vRAMPointer = 0;										
		} else {
			if (n != videoRAM[vRAMPointer]) {								// Has it changed ?
				dirtyFlag[vRAMPointer >> 2] = 1;							// Set the dirty flag
				screenNeedsPaint = 1;										// Set the repaint flag.
			}
			videoRAM[vRAMPointer] = n;										// otherwise it's a character
			vRAMPointer = (vRAMPointer + 1) & 0xFF;							// Bump video RAM pointer
		}
	}
	videoLast = n;															// update last written value.
}

// *******************************************************************************************************************************
//										Write bit to tape I/O
// *******************************************************************************************************************************

void HWIWriteTapeBit(BYTE8 bit) {
	#ifdef WINDOWS
	fputc(bit ? '1':'0',fTapeOut);
	#endif
}	

// *******************************************************************************************************************************
//										Read bit from tape I/O
// *******************************************************************************************************************************

BYTE8 HWIReadTapeBit(void) {
	#ifdef WINDOWS
	if (fTapeIn == NULL || feof(fTapeIn)) return 1; 						// 1 is default value.
	return fgetc(fTapeIn) & 1;
	#endif
	#ifdef ARDUINO
	return 1;
	#endif
}