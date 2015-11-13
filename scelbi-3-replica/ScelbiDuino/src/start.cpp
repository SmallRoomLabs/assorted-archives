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
  	CPUReset();
}

void loop()
{
	CPUExecuteSinglePhase();
}
