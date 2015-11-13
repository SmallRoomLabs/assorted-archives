// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.h
//		Purpose:	Hardware interface
//		Created:	4th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _HARDWARE_H
#define _HARDWARE_H

void HWIReset(void);
void HWIEndFrameUpdate(void);
void HWIWriteLEDDisplay(BYTE8 data);
void HWIWriteVideoMemory(BYTE8 x,BYTE8 y,BYTE8 isOn);
BYTE8 HWIReadKeyboard(void);
BYTE8 HWIReadToggles(void);
BYTE8 HWIReadControlKey(BYTE8 key);
void HWISetFrequency(WORD16 freq);

#define KEY_A 		(0x00)
#define KEY_B 		(0x01)
#define KEY_RESET	(0x02)

#ifdef WINDOWS
void DBGXWriteVDUMemory(int x,int y,int isOn);
void DBGXWriteLEDDisplay(int data);
int  HWIKeyMap(int key,int inRunMode);
#endif

#ifdef ARDUINO
#define MOSIpin A1 				// MOSI
#define MISOpin A3 				// Not actually used
#define SCLKpin A2 				// SCLK
#define SSpin A0 				// SS

const int DataPin = 2;			// PS2 Keyboard connections.
const int IRQpin =  3;

#define TONEPIN  12				// Beeper connections.

#endif 

#endif	