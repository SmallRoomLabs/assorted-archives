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

#ifndef _IOMANAGER_H
#define _IOMANAGER_H

void HWMReset(void);
BYTE8 HWMTransferWord(char direction,WORD16 *data);
void HWMExternalFunction(WORD16 functionCode);
BYTE8 HWMIsStatusPending(void);

#endif

