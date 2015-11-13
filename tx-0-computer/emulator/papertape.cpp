// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		papertape.cpp
//		Purpose:	Paper tape reader code.
//		Created:	11th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "devices.h"

#include <stdio.h>

static LONG32 _PTRReadWord(void);

static int ptrCount; 															// Number of paper tapes
static FILE *currentTapeFile = NULL;											// Current tape file handle.
static int nextTapeFile;														// Index of next file.
static char **tapeFiles; 														// Tape file names.

void PTRSetSource(int argc,char *argv[]) {
	printf("%d %s %s\n",argc,argv[0],argv[1]);
	ptrCount = argc;															// Save command line info.
	tapeFiles = argv;														
	currentTapeFile = NULL;														// No current file
	nextTapeFile = 1;															// Next is #1.

	LONG32 cmd,backLoad;
	do {
		cmd = _PTRReadWord();													// Read a 3 entry word from cmd.
		if ((cmd >> 16) == 0 || (cmd >> 16) == 3) {								// Write in if data.
			backLoad = _PTRReadWord();											// Read a 3 entry back load.
			CPUWrite(cmd & 0xFFFF,backLoad);
			//printf("%05o %06o\n",cmd & 0xFFFF,backLoad);
		}
	} while ((cmd >> 16) == 0 || (cmd >> 16) == 3);								// Until found add or trn.
	CPUSetPC(cmd & 0xFFFF);
}

BYTE8 PTRReadTape(void) {
	char szBuffer[32];

	if (currentTapeFile == NULL) {												// No current tape file
		if (nextTapeFile == ptrCount) return 0;									// Zero if nothing more.
		printf("Opening %s\n",tapeFiles[nextTapeFile]);
		currentTapeFile = fopen(tapeFiles[nextTapeFile],"r");					// Open the next paper tape file.
		nextTapeFile++;															// Bump it.
	}
	if (feof(currentTapeFile)) {												// Reached end of file ?
		fclose(currentTapeFile);												// Close that tape file.
		currentTapeFile = NULL;													// Null pointer
		return PTRReadTape();													// Call to get next.
	}
	fgets(szBuffer,sizeof(szBuffer),currentTapeFile);							// Read the next line.
	szBuffer[7] = '\0';															// Truncate it.
	BYTE8 ret = 0;
	for (BYTE8 n = 0;n < 7;n++) {
		if (szBuffer[n] == 'O') ret = ret | (0x40 >> n);
	}
	//printf("[%s %03o]\n",szBuffer,ret);

	return ret;
}

static LONG32 _PTRReadWord(void) {
	LONG32 w = 0;
	for (BYTE8 i = 0;i < 3;i++) {												// Three words
		w = w >> 1;																// Technically CYR, doesn't matter.
		BYTE8 n = PTRReadTape();												// Read the tape.
		if ((n & 1) != 0) {
			if ((n & 0100) != 0) w |= (0400000 >> 0);
			if ((n & 0040) != 0) w |= (0400000 >> 3);
			if ((n & 0020) != 0) w |= (0400000 >> 6);
			if ((n & 0010) != 0) w |= (0400000 >> 9);
			if ((n & 0004) != 0) w |= (0400000 >> 12);
			if ((n & 0002) != 0) w |= (0400000 >> 15);
		}
	}
	return w;
}
