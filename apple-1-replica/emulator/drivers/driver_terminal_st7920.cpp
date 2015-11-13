// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_terminal_st7920.cpp
//		Purpose:	Terminal Driver (ST7920 / Arduino)
//		Created:	21st October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "driver_terminal.h"
#include "ST7920LCDDriver.h"

const int MOSIpin = A1; // 11
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; 	// 10

static ST7920LCDDriver lcd(SCLKpin, MOSIpin, 0);

static int isInitialised = 0;
static BYTE8 buffer[6];

static void _Refresh(BYTE8 x,BYTE8 y) {
	for (BYTE8 n = 0;n < 4;n++) {
		buffer[n] = TRMRead(x+n,y);
	}
	lcd.drawText(x >> 2,y*6,buffer);
}

// *******************************************************************************************************************************
//													Basic Terminal for Debug Window
// *******************************************************************************************************************************

BYTE8 TRMCommand(BYTE8 command,BYTE8 p1,BYTE8 p2,BYTE8 p3) {
	if (isInitialised == 0) {
		pinMode(SSpin, OUTPUT);   
		digitalWrite(SSpin, HIGH);
		lcd.begin(true); 
		isInitialised = 1;         
	}

	BYTE8 retVal = 0;
	switch(command) {
		case TRC_GETWIDTH:
			retVal = 32;
			break;
		case TRC_GETHEIGHT:
			retVal = 10;
			break;
		case TRC_WRITE:											// Does nothing, updated in Debugger Display
			_Refresh(p1 & 0xFC,p2);								// Refresh on 4 character boundary.
			break;
		case TRC_CLEAR:											// Clears when rendered in debugger display.
			lcd.clear();
			break;

	}
	return retVal;
}
