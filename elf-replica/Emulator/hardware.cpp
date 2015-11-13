// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.cpp
//		Purpose:	Hardware Interface
//		Created:	19th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdlib.h>
#include "sys_processor.h"
#include "hardware.h"

#ifdef WINDOWS
#include "gfx.h"																// Want the keyboard access.
#endif

static WORD16 videoMemoryAddress = 0xFFFF;										// 1802 Video Memory Address
static BYTE8  screenIsOn = 0;													// 1861 turned on
static BYTE8  keypadLatch = 0;													// 74922 keyboard latch (Elf)
static BYTE8  ledDisplay = 0;													// 8 LED / 2 Digit display (Elf)
static BYTE8  *videoMemoryPointer;												// Physical screen memory ptr in SRAM

#ifdef ARDUINO
#include "SSD1306_OLED.h"														// Header for SPI LED driver
#include "Keypad.h"

static SSD1306ElfDisplay oLedDisplay(1);										// Create/initialise SSD1306
static BYTE8 displayingLEDS;													// True when SSD1306 displaying LED not Video

static const char keys[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'E','0','F','D'}
};

static byte rowPins[4] = {4,5,6,7}; 
static byte colPins[4] = { 8,9,2,3}; 
static Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, 4,4 );

#endif

// *******************************************************************************************************************************
//													Hardware Reset
// *******************************************************************************************************************************

void HWIReset(void) {
	#ifdef ARDUINO
	oLedDisplay.init();															// Initialise SSD1306
	oLedDisplay.clear();
	HWISetDigitDisplay(0);														// Clear binary display
	#endif
}

// *******************************************************************************************************************************
//											Process keys passed from debugger
// *******************************************************************************************************************************

#ifdef WINDOWS
BYTE8 HWIProcessKey(BYTE8 key,BYTE8 isRunMode) {
	if (isRunMode) {															// In run mode, push 0-9 A-F
		if (key >= '0' && key <= '9') 											// into keyboard latch.
			keypadLatch = (keypadLatch << 4) | (key - '0');
		if (key >= 'a' && key <= 'f')
			keypadLatch = (keypadLatch << 4) | (key - 'a' + 10);
	}
	return key;
}
#endif

// *******************************************************************************************************************************
//									Get/Set the 7 Segment Display And/Or LEDs
// *******************************************************************************************************************************

void HWISetDigitDisplay(BYTE8 led) {
	ledDisplay = led;
	#ifdef ARDUINO
	if (displayingLEDS == 0) oLedDisplay.clear();
	//oLedDisplay.displayBinary(led);
	displayingLEDS = 1;
	#endif	
}

BYTE8 HWIGetDigitDisplay(void) {
	return ledDisplay;
}

// *******************************************************************************************************************************
//							Get/Set the page address (1802 and Physical) for the video.
// *******************************************************************************************************************************

void HWISetPageAddress(WORD16 r0,BYTE8 *pointer) {
	videoMemoryAddress = r0;
	videoMemoryPointer = pointer;
}

WORD16 HWIGetPageAddress(void) {
	return videoMemoryAddress;
}

// *******************************************************************************************************************************
//											Get/Set the screen on flag
// *******************************************************************************************************************************

void HWISetScreenOn(BYTE8 isOn) {
	screenIsOn = (isOn != 0);
}
BYTE8 HWIGetScreenOn(void) {
	return screenIsOn;
}

// *******************************************************************************************************************************
//											  Check if IN is pressed
// *******************************************************************************************************************************

BYTE8 HWIIsInPressed(void) {
	#ifdef WINDOWS
	return (GFXIsKeyPressed(GFXKEY_RETURN) != 0);
	#else
	return 0;
	#endif
}

// *******************************************************************************************************************************
//											Read the 749C22 Keyboard Latch
// *******************************************************************************************************************************

BYTE8 HWIReadKeypadLatch(void) {
	return keypadLatch;
}

// *******************************************************************************************************************************
//												Called at End of Frame
// *******************************************************************************************************************************

void HWIEndFrame(void) {
	#ifdef ARDUINO
	if (screenIsOn && videoMemoryPointer != NULL) {								// Screen on and video memory assigned
		if (displayingLEDS) oLedDisplay.clear();								// Clear if displaying LEDs
		displayingLEDS = 0;														// Clear that flag.
		oLedDisplay.refresh(videoMemoryPointer);								// Refresh the screen
	}
	BYTE8 key = keypad.getKey();												// Check for key
	if (key != 0) {
		oLedDisplay.displayBinary(key);
		if (key >= '0' && key <= '9') 											// into keyboard latch.
			keypadLatch = (keypadLatch << 4) | (key - '0');
		if (key >= 'A' && key <= 'F')
			keypadLatch = (keypadLatch << 4) | (key - 'A' + 10);
	}
	#endif
}
