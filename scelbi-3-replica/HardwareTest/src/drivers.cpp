// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		drivers.c
//		Purpose:	Hardware interface
//		Created:	21st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "drivers.h"

static void _DRV20x4Initialise(void);
static void _DRV20x4RefreshPanel(WORD16 address,BYTE8 data,BYTE8 status,BYTE8 intMode,BYTE8 halt,BYTE8 runMode);

static BYTE8 toggles = 0;

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//														WINDOWS
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef WINDOWS

#include "gfx.h"
#define WRITEDISPLAY(x,y,c) DBGXWriteDisplay(x,y,c)

// *******************************************************************************************************************************
//											Check if one of the buttons is pressed
// *******************************************************************************************************************************

BYTE8 DRVIsPushButtonPressed(BYTE8 button) {
	return GFXIsKeyPressed(button);
}

// *******************************************************************************************************************************
//												Read the bank of toggle switches
// *******************************************************************************************************************************

BYTE8 DRVReadToggleSwitches(void) {
	return toggles;
}

// *******************************************************************************************************************************
//											Process keystrokes passed on during run time
// *******************************************************************************************************************************

BYTE8 DRVKeyMapper(BYTE8 key,BYTE8 inRunMode) {
	if (inRunMode != 0 && key >= '0' && key <= '7') {
		toggles = (toggles << 3) | (key & 7);
	}
	return key;
}

// *******************************************************************************************************************************
//													Refresh the display panel
// *******************************************************************************************************************************

void DRVRefreshPanel(WORD16 address,BYTE8 data,BYTE8 status,BYTE8 intMode,BYTE8 halt,BYTE8 runMode) {
	_DRV20x4RefreshPanel(address,data,status,intMode,halt,runMode);
}

// *******************************************************************************************************************************
//														Reset the hardware
// *******************************************************************************************************************************

#define DRVOFF_CHAR	(126)
#define DRVON_CHAR	(127)

void DRVReset(void) {
	GFXDefineCharacter(DRVON_CHAR,0x3e,0x7F,0x7F,0x7F,0x3E);
	GFXDefineCharacter(DRVOFF_CHAR,0x3e,0x41,0x41,0x41,0x3E);
	_DRV20x4Initialise();
}

// *******************************************************************************************************************************
//														End of frame driver
// *******************************************************************************************************************************

void DRVEndFrame(void) {
}

#endif

// *******************************************************************************************************************************
// *******************************************************************************************************************************
//														ARDUINO
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifdef ARDUINO

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);

#define WRITEDISPLAY(x,y,c) DRVAWriteLCD(x,y,c)

static void DRVAWriteLCD(BYTE8 x,BYTE8 y,BYTE8 c) {
	if (x < 20 && y < 4) lcd.setCursor(x,y);
	lcd.print((char)c);
}

// *******************************************************************************************************************************
//											Check if one of the buttons is pressed
// *******************************************************************************************************************************

BYTE8 DRVIsPushButtonPressed(BYTE8 button) {
	BYTE8 retVal = 0;
	switch(button) {
		case DRVKEY_INTERRUPT:	retVal = (digitalRead(APIN_INTERRUPT) == 0); break;
		case DRVKEY_STEP:		retVal = (digitalRead(APIN_STEP) == 0); break;
		case DRVKEY_RUN:		retVal = (digitalRead(APIN_RUN) == 0); break;
	}
	return retVal;
}

// *******************************************************************************************************************************
//												Read the bank of toggle switches
// *******************************************************************************************************************************

static const BYTE8 togglePins[] = { APIN_D7, APIN_D6, APIN_D5, APIN_D4, APIN_D3, APIN_D2, APIN_D1, APIN_D0 };

BYTE8 DRVReadToggleSwitches(void) {
	toggles = 0;
	for (BYTE8 i = 0;i < 8;i++) 
		if (digitalRead(togglePins[i]) == 0) toggles |= 0x80 >> i;
	return toggles;
}

// *******************************************************************************************************************************
//													Refresh the display panel
// *******************************************************************************************************************************

void DRVRefreshPanel(WORD16 address,BYTE8 data,BYTE8 status,BYTE8 intMode,BYTE8 halt,BYTE8 runMode) {
	_DRV20x4RefreshPanel(address,data,status,intMode,halt,runMode);
}

// *******************************************************************************************************************************
//														Reset the hardware
// *******************************************************************************************************************************

#define DRVOFF_CHAR	(2)
#define DRVON_CHAR	(1)

uint8_t onChar[8] = { 0x0E,0x1F,0x1F,0x1F,0x1F,0x1F,0x0E,0x00 };
uint8_t offChar[8] = { 0x0E,0x11,0x11,0x11,0x11,0x11,0x0E,0x00 };

void DRVReset(void) {
	for (BYTE8 i = 0;i < 8;i++) pinMode(togglePins[i],INPUT_PULLUP);
	pinMode(APIN_STEP,INPUT_PULLUP);
	pinMode(APIN_INTERRUPT,INPUT_PULLUP);
	pinMode(APIN_RUN,INPUT_PULLUP);
	lcd.init();                      
 	lcd.backlight();
 	lcd.createChar(DRVON_CHAR,onChar);
 	lcd.createChar(DRVOFF_CHAR,offChar);
	_DRV20x4Initialise();
}

// *******************************************************************************************************************************
//														End of frame driver
// *******************************************************************************************************************************

void DRVEndFrame(void) {
}

#endif

// *******************************************************************************************************************************
//												Generalised 20 x 4 Display Driver
// *******************************************************************************************************************************

#define WR(c) WRITEDISPLAY(0xFF,0xFF,c)

static void _DRV20x4Initialise(void) {
	WRITEDISPLAY(0,0,' ');
	for (BYTE8 i = 0;i < 79;i++) WRITEDISPLAY(0xFF,0xFF,' ');
	WRITEDISPLAY(4,0,'P');WR('a');WR('g');WR('e');
	WRITEDISPLAY(0,3,'R');
	WRITEDISPLAY(2,3,'I');WR('S');WR('t');
	WRITEDISPLAY(6,3,'H');
	WRITEDISPLAY(13,0,'A');WR('d');WR('d');WR('r');
	WRITEDISPLAY(13,3,'D');WR('a');WR('t');WR('a');
}

// *******************************************************************************************************************************
//												Draw Binary Number on 20x4 Display 
// *******************************************************************************************************************************

static void _DRV20x4Binary(BYTE8 x,BYTE8 y,BYTE8 data,BYTE8 width) {
	BYTE8 mask = 1 << (width - 1);
	while (width-- > 0) {
		WRITEDISPLAY(x++,y,(data & mask) ? DRVON_CHAR:DRVOFF_CHAR);
		mask = mask >> 1;
	}
}

// *******************************************************************************************************************************
//													Refresh the LCD Panel
// *******************************************************************************************************************************

static void _DRV20x4RefreshPanel(WORD16 address,BYTE8 data,BYTE8 status,BYTE8 intMode,BYTE8 halt,BYTE8 runMode) {
	_DRV20x4Binary(2,1,address >> 11,3);_DRV20x4Binary(6,1,address >> 8,3);
	_DRV20x4Binary(10,1,address >> 6,2);_DRV20x4Binary(13,1,address >> 3,3);_DRV20x4Binary(17,1,address,3);
	_DRV20x4Binary(10,2,data >> 6,2);_DRV20x4Binary(13,2,data >> 3,3);_DRV20x4Binary(17,2,data,3);
	_DRV20x4Binary(0,2,runMode,1);_DRV20x4Binary(2,2,intMode,1);
	_DRV20x4Binary(3,2,status,2);_DRV20x4Binary(6,2,halt,1);

	WRITEDISPLAY(0,0,(toggles >> 6) | '0');
	WRITEDISPLAY(1,0,((toggles >> 3) & 7) | '0');
	WRITEDISPLAY(2,0,(toggles & 7) | '0');
}
