// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		xd_tape_dummy.cpp
//		Purpose:	External Drivers : Dummy Cassette I/O
//		Created:	28th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include "drivers.h"

void XDTapeInit(void) {
}

BYTE8 XDTapeReadBit(void) {
	return 1;
}

void XDTapeWriteBit(BYTE8 bit) {
}
