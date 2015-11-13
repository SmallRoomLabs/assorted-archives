// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		hardware.c
//		Purpose:	Hardware handling routines (Scelbi specific)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"

BYTE8 CPUXReadPort(BYTE8 portID) { return 0; }
void CPUXWritePort(BYTE8 portID,BYTE8 data) {}
void CPUXUpdateDisplay(BYTE8 leftStat,BYTE8 rightStat,WORD16 address,BYTE8 data) {}