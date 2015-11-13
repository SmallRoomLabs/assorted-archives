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
#include "gfx.h"

// *******************************************************************************************************************************
//
//	Keyboard Mapping:
//
//	$30-$3F 	(0-9 etc.) 		No bits set
//	$40-$4F 	(@,A-O)			Bit 4 set (Alpha 1)
//	$50-$5F 	(P-Z etc.)		Bit 5 set (Alpha 2)
//	$20-$2F 	(space,",etc)	Bit 6 set (Punc)
//
//	Read Addr
//	=========
//	Bit 3 		Keys 0,1,2,3 	Sets bit 0,1,2,3 respectively.
//	Bit 2 		Keys 4,5,6,7
//	Bit 1 		Keys 8,9,:,;
//	Bit 0 		Keys <,=,>,?
//
// *******************************************************************************************************************************

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//							Helper function maps ASCII key onto keyboard port value
// *******************************************************************************************************************************
// *******************************************************************************************************************************

static BYTE8 HWIMapASCIIToKeyboardPort(BYTE8 key,BYTE8 address) {
	BYTE8 port = 0;
	address = address & 0x0F; 														// Only interested in bits 0-3.
	if (key >= 'a' && key <= 'z') key = key - 32;									// Make it upper case.
	if (key < 0x20 || key > 0x5F || address == 0) return 0; 						// If out of range, or no address bits,return 0.
	if ((key >> 4) == 0x4) port = 0x10; 											// 40-4F set bit 4
	if ((key >> 4) == 0x5) port = 0x20; 											// 50-5F set bit 5
	if ((key >> 4) == 0x2) port = 0x40; 											// 20-2F set bit 6
	key = key & 0x0F;																// Only interested in the lower 4 bits.
	if ((address & 8) != 0 && key < 4) port = port | (1 << (key & 3));
	if ((address & 4) != 0 && key >= 4 && key < 8) port = port | (1 << (key & 3));
	if ((address & 2) != 0 && key >= 8 && key < 12) port = port | (1 << (key & 3));
	if ((address & 1) != 0 && key >= 12) port = port | (1 << (key & 3));
	return port;
}

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//													Windows Specific Code
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS

static BYTE8 currentKey = 0x00;														// Current key pressed, $FF if none.

// *******************************************************************************************************************************
//														Read the keyboard port.
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboard(BYTE8 address) {
	if (currentKey == 0xFF) return 0x00; 											// No key pressed
	return HWIMapASCIIToKeyboardPort(currentKey,address);							// Map ASCII onto keyboard port value
}

// *******************************************************************************************************************************
//													 Is the INT/Return key pressed
// *******************************************************************************************************************************

BYTE8 HWIIsIntPressed(void) {
	return GFXIsKeyPressed(GFXKEY_RETURN);
}

// *******************************************************************************************************************************
//											Update the current keyboard key at end of frame
// *******************************************************************************************************************************

void HWIEndFrameUpdate(void) {
	currentKey = 0xFF;																// Work out which key is pressed
	for (int i = 0x20;i < 0x60;i++) {
		if (GFXIsKeyPressed(i)) currentKey = i;
	}
	if (currentKey != 0xFF && GFXIsKeyPressed(GFXKEY_SHIFT)) { 						// Handle shifts
		switch (currentKey) {
			case '-': currentKey = '_';break;										// Many special cases, different keyboards
			case '=': currentKey = '+';break;
			case ';': currentKey = ':';break;
			case '#': currentKey = '~';break;
			case ',': currentKey = '<';break;
			case '.': currentKey = '>';break;
			case '/': currentKey = '?';break;
			case '@': currentKey = '\'';break;
			default:																// Top row above numbers.
				if (currentKey >= '0' && currentKey <= '9') {
							currentKey = ")!\"#$%^&*()"[currentKey-'0'];
				}
				break;
			}
	}
}

#endif

