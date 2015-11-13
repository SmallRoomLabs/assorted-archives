// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		interface.c
//		Purpose:	Scelbi controls/display interface
//		Created:	10th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "interface.h"
#include "gfx.h"

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//														WINDOWS Interface
//	
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS

static BYTE8 statuses[32];															// Output statuses.
static BYTE8 lastKeyboardState = 0; 												// Previous keyboard state.

// *******************************************************************************************************************************
//									Process keys from the debugger in RUN mode
// *******************************************************************************************************************************

int DBGXProcessDebugKey(int pendingKey,int runMode) {
	return pendingKey;
}

// *******************************************************************************************************************************
//											Reset or Initialise the Interface
// *******************************************************************************************************************************

void IFReset(void) {
	IFWrite(IFW_HOGENSON,0201);														// Turn display off.
}

// *******************************************************************************************************************************
//										Get key in ASCII , ECS-5 specification 
// *******************************************************************************************************************************

static BYTE8 _IFGetKeyboard() {
	BYTE8 key = 0xFF;																// $FF is no key
	for (BYTE8 t = 0;t < 0x7F;t++)													// Figure out which key
		if (GFXIsKeyPressed(t)) key = t;
	if (key < 0x20 && key != GFXKEY_RETURN) key = 0xFF;								// Only allow RETURN control key.
	if (key != 0xFF) {
		key = GFXToASCII(key,1);													// ASCII at this point
		if (key >= 'a' && key <= 'z') key = key - 32;								// Make Upper Case
		if (key == 0x0D) key = 0xFE;												// CR maps onto the > key.
		key = (key & 0x3F) | 0xC0;													// Make 6 bit, set upper 2
		if (GFXIsKeyPressed(GFXKEY_CONTROL)) key = key & 0x7F;						// Bit 7 cleared by CTRL
		if (GFXIsKeyPressed(GFXKEY_SHIFT)) key = key & 0xBF;						// Bit 6 cleared by SHIFT.

		BYTE8 six = key & 0x3F;														
		if ((six >= 0x21 && six <= 0x30) || (six >= 0x3A)) {						// Shifted keys
			key = key | 0x40;														// We pretend they aren't.
		}
	}
	return key;
}

//	Does not support Shift + ! because Shift has to pressed on every keyboard I know of to access !
//	The routine *pretends* that shift isn't pressed if you have this key, because the design has it 
//	as a key on its own.

// *******************************************************************************************************************************
//													Read control items
// *******************************************************************************************************************************

BYTE8 IFRead(BYTE8 item) {
	BYTE8 newKbd,retVal = 0;
	switch(item) {
		case IFR_KEYBOARD:
			retVal = _IFGetKeyboard();												// Read the keyboard.
			break;
		case IFR_INTERRUPTQUERY:
			newKbd = _IFGetKeyboard();												// Scan keyboard
			retVal = (newKbd != lastKeyboardState && lastKeyboardState == 0xFF);	// Yes if keyboard pressed
			lastKeyboardState = newKbd;												// update last state
			break;
		default:
			retVal = statuses[item];break;
	}
	return retVal;
}

// *******************************************************************************************************************************
//					Write to control items. We don't actually have any hardware so we just remember them.
// *******************************************************************************************************************************

void IFWrite(BYTE8 item,BYTE8 data) {
	statuses[item] = data;
	switch(item) {
		case IFW_LEFTDISPLAY:
			break;
		case IFW_RIGHTDISPLAY:
			break;
		case IFW_HOGENSON:
			HWIWriteHogensonDisplay(data);
			break;
	}
}

#endif

