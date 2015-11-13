// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		devices.h
//		Purpose:	Include file for devices.
//		Created:	11th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _DEVICES_H
#define _DEVICES_H

#define 	DV_SCOPE 		(1)														// Device IDs
#define 	DV_PRINTER		(2)
#define 	DV_KEYBOARD		(3)
#define 	DV_PTPUNCH 		(4)
#define 	DV_PTREADER		(5)
#define 	DV_LIGHTPEN1	(6)
#define 	DV_LIGHTPEN2 	(7)
#define 	DV_TOGGLESWACC	(8)
#define 	DV_TOGGLESWBUF	(9)

void PTRSetSource(int argc,char *argv[]);
BYTE8 PTRReadTape(void);

#endif