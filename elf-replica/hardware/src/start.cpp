#include <Arduino.h>
#include "sys_processor.h"
#include "hardware.h"

void setup()
{
  	CPUReset();
}

unsigned long nextFrameTime = 0;

void loop()
{
    unsigned long frameRate = CPUExecuteInstruction();
    if (frameRate != 0) {
		while (millis() < nextFrameTime) {}
		nextFrameTime = nextFrameTime + 1000 / frameRate;
	}
}
