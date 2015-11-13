// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		drivers.h
//		Purpose:	Target dependent code (header)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _DRIVERS_H
#define _DRIVERS_H

int DRVProcessDebugKey(int key,int isRuntime);
void DRVInitialise(void);
void DRVWriteDisplay(BYTE8 address,BYTE8 data);
BYTE8 DRVGetNextKey(void);
void DRVEndFrame(void);

#endif