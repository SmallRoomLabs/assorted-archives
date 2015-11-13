// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (Scelbi specific)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "drivers.h"

static BYTE8 	currentKey = 0x00; 													// Current pressed key.

static char scopeWriterBuffer[33];													// 32 characters.
static BYTE8 scopeAddress;															// Address of write.

// *******************************************************************************************************************************
//													Reset all hardware
// *******************************************************************************************************************************

void HWIReset(void) {
	DRVInitialise();																// Initialise the driver
	for (BYTE8 i = 0;i < 32;i++) scopeWriterBuffer[i] = '*';						// Clear SCW Buffer to '*'
	scopeWriterBuffer[32] = '\0';
}

// *******************************************************************************************************************************
//														Read keyboard
// *******************************************************************************************************************************

BYTE8 HWIReadKeyboard(void) {
	return currentKey;
}

// *******************************************************************************************************************************
//										Write to Scopewriter (partial emulation only)
// *******************************************************************************************************************************

void HWIWriteScopewriter(BYTE8 reg,BYTE8 data) {
	//
	//	This is nowhere near complete, it is just enough to work with the Scelbi interface code.
	//
	if (reg == SCOPEWRITER_WRITE && data == 040) scopeAddress = 0; 					// Writing 040o to Write resets addr
	if (reg == SCOPEWRITER_DATA) {													// Writing to data.
		data = (data & 0x3F); 														// 6 bit ASCII
		if (data < 0x20) data |= 0x40;												// Make 7 bit ASCII
		scopeWriterBuffer[scopeAddress] = data;										// Copy into buffer.
		scopeAddress = (scopeAddress + 1) & 0x1F;									// Bump the write pointer.
	}
}

// *******************************************************************************************************************************
//													Handle on end of frame.
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	BYTE8 newKey = DRVGetKeyIfAvailable();											// New key press ?
	if (newKey != 0) {																// If so, update current key
		currentKey = newKey;
		CPURequestInterrupt();														// and ask for an interrupt.
	}
	DRVEndFrame();
	DRVWriteScopewriter(scopeWriterBuffer);											// Update scope writer display.
}

