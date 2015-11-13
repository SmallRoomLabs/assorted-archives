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

BYTE8 DRVGetKeyIfAvailable(void);
void DRVWriteScopewriter(char *text);
int DRVProcessDebugKey(int key,int isRunMode);
void DRVInitialise(void);
void DRVEndFrame(void);

#endif