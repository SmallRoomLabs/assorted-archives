#include <Arduino.h>
#include "sys_processor.h"

void setup() {
	CPUReset();
}

unsigned long nextFrameTime = 0;

void loop() {
	for (BYTE8 n = 0;n < 200;n++) {
    	unsigned long frameRate = CPUExecuteInstruction();
    	if (frameRate != 0) {
    		while (millis() < nextFrameTime) {}
    		nextFrameTime = nextFrameTime + 1000 / frameRate;
    	}	
	}
}
