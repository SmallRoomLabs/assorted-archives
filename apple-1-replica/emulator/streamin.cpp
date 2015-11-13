// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		streamin.cpp
//		Purpose:	Input Stream for Emulator programs.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include "sys_processor.h"

FILE *fStream;
static const char *fileName = NULL;

void IOSetStreamFile(char *name) {
	fileName = name;
}

BYTE8 IOIsStreamLoading(void) {
	return fileName != NULL;
}

BYTE8 IOReadInputStream(void) {
	if (fileName == NULL) return 0;
	if (fStream == NULL) fStream = fopen(fileName,"r");
	if (fStream == NULL || feof(fStream)) {
		fileName = NULL;
		return 0;
	}
	BYTE8 c = fgetc(fStream);
	return c;
}
