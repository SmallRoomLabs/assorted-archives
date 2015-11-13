// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		iomanager.h
//		Purpose:	CDC160 Processor Emulation - Hardware Manager (Header)
//		Created:	2nd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "iomanager.h"

static BYTE8 	currentDevice = 0x00;												// Currently selected device.
static BYTE8 	statusPending = 0; 													// Status pending after EXF ?

// *******************************************************************************************************************************
//												Reset Hardware Interface
// *******************************************************************************************************************************

void HWMReset(void) {
	//printf("HW:Reset\n");
	currentDevice = 0x00;															// No current device
	statusPending = 0x00; 															// No status pending.
}

// *******************************************************************************************************************************
//
//	Device commands:-
//
//	F 		External function 	pointer to command word, lower 6 bits only. Returns non-zero if status request.
//	I 		Input a word 		read a word from the device.
//	O 		Write a word 		write word to device.
//	Q 		Query Input 		is device ready to send (for keyboard i/p, true when line available)
//	S 		Read status 		Read device status.	
//
// *******************************************************************************************************************************

#ifdef WINDOWS
#include <stdio.h>
#define WARN(t,m) if ((t)) printf("WARN:%s\n",m)
#else
#define WARN(t,m) {}
#endif

// *******************************************************************************************************************************
//													Typewriter device
// *******************************************************************************************************************************


struct _typewriter {
	BYTE8 activeDevice;																// Current active device.
	BYTE8 isDataAvailable;															// Non zero when data available.
	BYTE8 data[80];																	// Data available for input (ASCIIZ)
	BYTE8 dataPointer; 																// Pointer into data for input.
} typeWriterStatus;

BYTE8 DVCTypewriter(BYTE8 deviceID,char fn,WORD16 *pData) {

	BYTE8 retVal = 0;	
	printf("Typewriter: %04o %c %04o\n",deviceID,fn,*pData);

	switch(fn) {
		case 'F':	if (*pData == 040) retVal = 1;									// Status request 4240
					else typeWriterStatus.activeDevice = *pData; 					// otherwise set active device.
					break;

		case 'S':	*pData = 0;														// Status request is always okay.
					break;

		case 'Q':	retVal = typeWriterStatus.isDataAvailable;						// Is data available ?
					break;														

		case 'I':	WARN(typeWriterStatus.activeDevice != 010,"Typewriter:Output not selected");
					WARN(typeWriterStatus.isDataAvailable == 0,"No data available");
					if (typeWriterStatus.data[typeWriterStatus.dataPointer] == 0) {	// No more data availeable
						retVal = 1;													// Return error 
						printf("Data read failed.\n");
					} else {														// Otherwise get next from input buffer.
						retVal = typeWriterStatus.data[typeWriterStatus.dataPointer++];
						printf("Read %c %o\n",retVal,retVal);
					}
					break;

		case 'O':	WARN(typeWriterStatus.activeDevice != 010,"Typewriter:Output not selected");
					printf("Transmit %c %o\n",*pData,*pData);
					break;

	}
	return retVal;
}

// *******************************************************************************************************************************
//											Transfer a word to or from the device
// *******************************************************************************************************************************

BYTE8 HWMTransferWord(char direction,WORD16 *data) {
	BYTE8 error = 0xFF;
	if (direction == 'I' && statusPending != 0) direction = 'S';					// Reading the status.
	statusPending = 0;																// No longer a status pending.
	switch(currentDevice) {															// Dispatch appropriately.
		case 042:																	// 43 is the attached teletype.
			error = DVCTypewriter(currentDevice,direction,data);			
			break;
	}
	//printf("HW:Transfer [%02o] %c %04o <%o>\n",currentDevice,direction,*data,error);
	return error;
}

// *******************************************************************************************************************************
//				Call external function code, select device, send function code, ask for status next input
// *******************************************************************************************************************************

void HWMExternalFunction(WORD16 functionCode) {
	WORD16 cmd = (functionCode & 077);
	//printf("HW:Function %04o\n",functionCode);
	currentDevice = (functionCode >> 6);											// Select current device.
	WORD16 func = HWMTransferWord('F',&cmd);										// Send to device.
	statusPending = (func != 0);													// Require status ?
}

// *******************************************************************************************************************************
//											Is a status read pending ?
// *******************************************************************************************************************************

BYTE8 HWMIsStatusPending(void) {
	return statusPending;
}