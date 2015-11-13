// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_common.h
//		Purpose:	Common Driver Functions
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _DRIVER_COMMON_H
#define _DRIVER_COMMON_H
#include "sys_processor.h"

typedef BYTE8 DRVPARAM;

DRVPARAM DRVRead(BYTE8 command,DRVPARAM data);							// Read (keyboard/tape) driver
void 	 DRVWrite(BYTE8 command,DRVPARAM data);							// Write (display/tape) driver

#define DWA1_WRITE			(0)											// Write character to terminal (valid ASCII)
#define DWA1_BACKSPACE		(1)											// Terminal backspace
#define DWA1_NEWLINE		(2) 										// New line
#define DRA1_KEYBOARD		(0) 										// Returns 00 (no key) or ASCII key
																		// 08 BackSpace 09 Tab 0D Return
#define DRA1_ISRESET		(1)											// Non zero if should reset

#define DRA1_RESET			(0xFF)										// Any reset actions.
#define DWA1_RESET			(0xFF)
#define DWA1_FRAME			(0xFE)										// End of frame
#define DRA1_FRAME			(0xFE)

#endif

