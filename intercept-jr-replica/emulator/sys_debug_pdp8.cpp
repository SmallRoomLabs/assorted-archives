// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_pdp8.cpp
//		Purpose:	Debugger Code (System Dependent)
//		Created:	3rd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gfx.h"
#include "sys_processor.h"
#include "sys_debug_system.h"
#include "debugger.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

// *******************************************************************************************************************************
//												Reset the 8008
// *******************************************************************************************************************************

void DBGXReset(void) {
	CPUReset();
}

static const char *_DBGXDecodeOper(int opcode) {
	const char *mnemonic = NULL;
	switch(opcode) {
		#include "__im6100_mnemonics.h"
	}
	return mnemonic;
}
// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

void DBGXRender(int *address,int showDisplay) {

	int n;
	char buffer[32];
	GFXSetCharacterSize(44,23);

	CPUSTATUS *s = CPUGetStatus();													// Get the CPU Status

	const char *labels[] = { "AC","LK","MQ","PC","HA","CY","BP","","AD","DA",NULL };
	const char *opcodes[] = { "and","tad","isz","dca","jms","jmp","opr","iot" };

	n = 0;
	while (labels[n] != NULL) {
		GFXString(GRID(17,n),labels[n],GRIDSIZE,DBGC_ADDRESS,-1);
		n++;
	}

	n = address[1];																	// Dump memory.
	for (int row = 14;row < 23;row++) {
		GFXNumber(GRID(0,row),n & 07777,8,4,GRIDSIZE,DBGC_ADDRESS,-1);			// Head of line
		GFXCharacter(GRID(4,row),':',GRIDSIZE,DBGC_HIGHLIGHT,-1);
		for (int col = 0;col < 8;col++) {											// Data on line
			GFXNumber(GRID(5+col*5,row),CPURead(n & 07777),8,4,GRIDSIZE,DBGC_DATA,-1);
			n++;
		}
	}

	#define DD(value,width)	GFXNumber(GRID(20,n++),value,8,width,GRIDSIZE,DBGC_DATA,-1)

	n = 0;
	DD(s->ac,4);DD(s->l,1);DD(s->mq,4);DD(s->pc,4);DD(s->halt,1);
	DD(s->cycles,4);DD(address[3],4);
	n++;
	DD(0,3);DD(0,3);

	n = address[0]; 																// Dump code.
	for (int row = 0;row < 13;row++) {
		int isPC = (n & 07777) == (s->pc);											// Check for breakpoint and being at PC
		int isBrk = ((n & 07777) == address[3]);
		GFXNumber(GRID(0,row),n & 07777,8,4,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_ADDRESS,isBrk ? 0xF00 : -1);
		int instr = CPURead(n & 07777);
		sprintf(buffer,"%s %03o",opcodes[instr >> 9],instr & 0777);
		if ((instr >> 9) < 6) {														// Not OPR or IOT
			buffer[4] = '\0';														// Trunc after space
			if ((instr & 00400) != 0) strcat(buffer,"I ");							// Add I if indirect
			sprintf(buffer+strlen(buffer),"%04o",									// Address from Z or Page
				((instr & 00200) != 0) ? (n & 07600) | (instr & 00177) : instr & 0177);
		}
		const char *alt = _DBGXDecodeOper(instr);									// Does a shorthand exist ?
		if (alt != NULL) strcpy(buffer,alt);
		GFXString(GRID(5,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_DATA,-1);
		n = (n + 1) & 07777;
	}

}	
