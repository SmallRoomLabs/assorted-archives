// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//      Name:       hardware.h
//      Purpose:    Hardware interface
//      Created:    4th September 2015
//      Author:     Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <Arduino.h>
#include <PS2Keyboard.h>
#include "sys_processor.h"
#include "hardware.h"

unsigned long nextFrameTime;

void setup() {
    CPUReset();
}


void loop() {

    unsigned long frameRate = CPUExecuteInstruction();
    if (frameRate != 0) {
    	while (millis() < nextFrameTime) {}
    	nextFrameTime = nextFrameTime + 1000 / frameRate;
    }
}

// End

