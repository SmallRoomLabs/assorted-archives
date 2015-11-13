// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (DGM8 specific)
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "drivers.h"

static BYTE8 videoRAM[256];													// 32 x 8 TVT Memory
static BYTE8 isInitialised = 0;												// Non zero when initialised
static BYTE8 vRAMPointer = 0;												// offset into video RAM
static BYTE8 videoLast = 0;													// Last value written to TVT
static BYTE8 pendingKey = 0;												// Key ready ?

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	if (isInitialised == 0) {
		isInitialised = 1;
		for (WORD16 n = 0;n < 256;n++) videoRAM[n] = 0; 					// Clear VRAM (wouldn't happen)
		pendingKey = 0;														// Clear keyboard buffer
		XDKeyboardInit();													// Initialise drivers
		XDTapeInit();
		XDDisplayInit();
	}
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	XDDisplayRepaint();
	BYTE8 newKey = XDKeyboardGetNewKey();									// Any key presses ?
	if (newKey != 0) pendingKey = newKey;									// Yes, put in buffer (1 level)
}

// *******************************************************************************************************************************
//														Read video memory ptr
// *******************************************************************************************************************************

BYTE8 *HWIGetVideoMemory() {
	return videoRAM;
}

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
			videoRAM[vRAMPointer] = n;										// otherwise it's a character
			XDDisplayWrite(vRAMPointer,n);									// Notify driver
			vRAMPointer = (vRAMPointer + 1) & 0xFF;							// Bump video RAM pointer
		}
	}
	videoLast = n;															// update last written value.
}

// *******************************************************************************************************************************
//										Write bit to tape I/O
// *******************************************************************************************************************************

void HWIWriteTapeBit(BYTE8 bit) {
	XDTapeWriteBit(bit);
}	

// *******************************************************************************************************************************
//										Read bit from tape I/O
// *******************************************************************************************************************************

BYTE8 HWIReadTapeBit(void) {
	return XDTapeReadBit();
}