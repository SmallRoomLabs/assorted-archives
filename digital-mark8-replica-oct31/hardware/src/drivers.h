// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		drivers.h
//		Purpose:	External Drivers (header)
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _DRIVERS_H

#include "sys_processor.h"

void 	XDKeyboardInit(void);
BYTE8 	XDKeyboardGetNewKey(void);

void 	XDTapeInit(void);
BYTE8 	XDTapeReadBit(void);
void 	XDTapeWriteBit(BYTE8 bit);

void 	XDDisplayInit(void);
void 	XDDisplayWrite(BYTE8 address,BYTE8 data);
void 	XDDisplayRepaint(void);

#define _DRIVERS_H
#endif

