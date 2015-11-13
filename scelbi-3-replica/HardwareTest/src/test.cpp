//
//	Test for Arduino hardware
//
//	Displays dummy panel on LCD
//	Toggle switch values are shown top left
//	Interrupt, Step and Run are bits 2,1,0 of the address.
//
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "sys_processor.h"
#include "drivers.h"

void setup()
{

  DRVReset();
}

void loop()
{
	int n = 0x3E50;
	if (DRVIsPushButtonPressed(DRVKEY_INTERRUPT)) n |= 4;
	if (DRVIsPushButtonPressed(DRVKEY_STEP)) n |= 2;
	if (DRVIsPushButtonPressed(DRVKEY_RUN)) n |= 1;
	DRVReadToggleSwitches();
	DRVRefreshPanel(n,0xA5,0x2,1,1,1);
}
