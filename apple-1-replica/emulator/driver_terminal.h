// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_terminal.h
//		Purpose:	Output Driver, General Terminal Interface (Header)
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _DRIVER_TERMINAL_H
#define _DRIVER_TERMINAL_H

#define TRC_GETWIDTH		(0)									// Retrieves width in characters
#define TRC_GETHEIGHT		(1)									// Retrieves height in characters
#define TRC_WRITE			(2)									// Write character p3 at cell (p1,p2)
#define TRC_CLEAR			(3)									// Clear the screen
#define TRC_XSTEP			(4) 								// X Step when scrolling refresh - because (say) the
																// ST7920 updates in blocks of 4 characters.
																
BYTE8 TRMCommand(BYTE8 command,BYTE8 p1,BYTE8 p2,BYTE8 p3);		// Call the terminal driver.
BYTE8 TRMRead(BYTE8 x,BYTE8 y);									// Information request, read character at (x,y)

#endif
