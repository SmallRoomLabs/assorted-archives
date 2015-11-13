// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_terminal.cpp
//		Purpose:	Output Driver, General Terminal Interface.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdlib.h>
#include "sys_processor.h"
#include "driver_terminal.h"
#include "driver_common.h"

static BYTE8 isInitialised = 0;														// Non zero when initialised
static BYTE8 width,height;															// Width and height of terminal.
static BYTE8 *terminalData = NULL;													// Terminal data (width * height)
static BYTE8 xCursor,yCursor;														// Write position
static BYTE8 cursorState = 0;														// Cursor state
static LONG32 nextCursorSwitch = 0;													// Time in ms of next switch.

// *******************************************************************************************************************************
//												Clear the Screen
// *******************************************************************************************************************************

static void _DTClearScreen(void) {
	for (WORD16 n = 0;n < width * height;n++) terminalData[n] = ' ';				// Fill mirror with spaces.
	xCursor = yCursor = 0;															// Home Cursor
	TRMCommand(TRC_CLEAR,0,0,0);													// Clear Screen
}

// *******************************************************************************************************************************
//										  Write to display and mirror
// *******************************************************************************************************************************

static void _DTWrite(BYTE8 x,BYTE8 y,BYTE8 c) {
	terminalData[x+y*width] = c;
	TRMCommand(TRC_WRITE,x,y,c);
}

// *******************************************************************************************************************************
//											Scroll terminal screen
// *******************************************************************************************************************************

static void _DTScroll(void) {
	for (BYTE8 x = 0;x < width;x++) {												// First scroll the mirror data
		for (BYTE8 y = 0;y < height-1;y++) {
			terminalData[x+y*width] = terminalData[x+(y+1) * width];
		}
		_DTWrite(x,height-1,' ');													// Clear newly scrolled line.
	}
	BYTE8 xStep = TRMCommand(TRC_XSTEP,0,0,0);										// Read x Step
	if (xStep == 0) xStep = 1;														// Default to 1
	for (BYTE8 x = 0;x < width;x = x + xStep) {										// Then copy it back out.
		for (BYTE8 y = 0;y < height;y++) {											// step is for devices like ST7920
			TRMCommand(TRC_WRITE,x,y,terminalData[x+y*width]); 						// Update horizontally in 16 pixel words
		}
	}
}

// *******************************************************************************************************************************
//									External terminal display read function
// *******************************************************************************************************************************

BYTE8 TRMRead(BYTE8 x,BYTE8 y) {
	if (terminalData == NULL || x >= width || y >= height) return '?';
	WORD16 addr = width;
	addr = addr * y + x;
	return terminalData[addr];
}

// *******************************************************************************************************************************
//												Terminal Driver
// *******************************************************************************************************************************

void DRVWrite(BYTE8 command,DRVPARAM data) {
	if (isInitialised == 0) {
		isInitialised = 1;
		width = TRMCommand(TRC_GETWIDTH,0,0,0);										// Get the size of terminal
		height = TRMCommand(TRC_GETHEIGHT,0,0,0);
		terminalData = (BYTE8 *)malloc(height * width);								// Allocate memory
		_DTClearScreen();
	}

	switch(command) {
		case DWA1_WRITE:	
			_DTWrite(xCursor,yCursor,data);											// Write out.		
			xCursor = (xCursor + 1) % width;										// Increment x position.
			if (xCursor == 0) {														// New line, bump Y
				yCursor++;
				if (yCursor == height) {											// Scroll ?
					_DTScroll();
					yCursor = height-1;												// Cursor on bottom line.
				}
			}
			break;

		case DWA1_BACKSPACE:
			_DTWrite(xCursor,yCursor,' ');											// Erase Cursor
			if (xCursor > 0) {														// Can we backspace ?
				xCursor--;
				_DTWrite(xCursor,yCursor,' ');										// Erase old character.
			}
			break;

		case DWA1_NEWLINE:	
			while (xCursor != 0) DRVWrite(DWA1_WRITE,' ');						
			break;

		case DWA1_FRAME:	
			if (SYSMilliseconds() > nextCursorSwitch) {
				nextCursorSwitch = SYSMilliseconds() + 500;
				cursorState = (cursorState == 0);									// Toggle state
				_DTWrite(xCursor,yCursor,cursorState ? '@':' ');					// Update
			}
			break;

		case DWA1_RESET:	
			break;
	}
}

