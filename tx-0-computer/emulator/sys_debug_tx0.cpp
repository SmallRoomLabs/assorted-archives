// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_tx0.c
//		Purpose:	Debugger Code (System Dependent)
//		Created:	21st September 2015
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
#include "__tx0_python_opr.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

struct __OperatorCommand {															// Contains known OPRs.
	int code;
	char command[4];
} operators[128];

int operatorCount = 0;																// Number of operators

static BYTE8 ttyDisplay[TTY_DISPWIDTH * TTY_DISPHEIGHT];							// TTY Display Memory.									

// *******************************************************************************************************************************
//										  This renders the debug screen
// *******************************************************************************************************************************

//	s.status = status;s.interruptMode = interruptMode;								// Internal statuses.
//	s.singleStepMode = singleStepMode;s.cpuPhase = cpuPhase;

void DBGXRender(int *address,int showDisplay) {
	if (operatorCount == 0) {														// If fist time unpack operators.
		const char *p = OPERATOR_LIST;												// List of operators.
		while (*p != '\0') {
			operators[operatorCount].command[0] = *p++;
			operators[operatorCount].command[1] = *p++;
			operators[operatorCount].command[2] = *p++;
			operators[operatorCount].command[3] = '\0';
			operators[operatorCount].code = atoi(p+1);
			p = p + strlen(p)+1;
			operatorCount++;
		}
		for (int i = 0;i < TTY_DISPHEIGHT * TTY_DISPWIDTH;i++) ttyDisplay[i] = ' ';	// Clear display.
		ttyDisplay[0] = ' '|0x80;													// Home cursor.
	}
	int n,x,y;
	char buffer[32];
	GFXSetCharacterSize(36,24);

	CPUSTATUS *s = CPUGetStatus();													// Get the CPU Status

	const char *labels1[] = { "AC:","LR:","CY:",NULL };
	const char *labels2[] = { "MB:","PC:","HA:",NULL };
	const char *mnemonics[] = { "sto","add","trn","opr" };

	DBGVerticalLabel(19,0,labels1,DBGC_ADDRESS,-1);
	DBGVerticalLabel(29,0,labels2,DBGC_ADDRESS,-1);

	#define DD(v,w) GFXNumber(GRID(x,y++),v,8,w,GRIDSIZE,DBGC_DATA,-1)

	x = 22;y = 0;DD(s->ac,6);DD(s->lr,6);DD(s->cycles,6);
	x = 32;y = 0;DD(s->mbr,6);DD(s->pc,6);DD(s->halt,1);
	n = address[1];																	// Dump memory.
	for (int row = 12;row < 23;row++) {
		GFXNumber(GRID(2,row),n & 0xFFFF,8,6,GRIDSIZE,DBGC_ADDRESS,-1);			// Head of line
		GFXCharacter(GRID(8,row),':',GRIDSIZE,DBGC_HIGHLIGHT,-1);
		for (int col = 0;col < 4;col++) {											// Data on line
			GFXNumber(GRID(9+col*7,row),CPURead(n & 0xFFFF),8,6,GRIDSIZE,DBGC_DATA,-1);
			n++;
		}
	}

	n = address[0]; 																// Dump code.
	for (int row = 0;row < 11;row++) {
		int isPC = (n & 0xFFFF) == (s->pc);											// Check for breakpoint and being at PC
		int isBrk = ((n & 0xFFFF) == address[3]);
		GFXNumber(GRID(0,row),n & 0xFFFF,8,6,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_ADDRESS,isBrk ? 0xF00 : -1);
		int opcode = CPURead(n);
		sprintf(buffer,"%s %06o",mnemonics[opcode >> 16],opcode & 0xFFFF);
		if (opcode >= 0600000) {													// Check for known operators
			for (int i = 0;i < operatorCount;i++)
				if (operators[i].code == opcode) strcpy(buffer,operators[i].command);
		}
		GFXString(GRID(7,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_DATA,-1);
		n++;
	}

	for (int col = 0;col < TTY_DISPWIDTH;col++)
		for (int row = 0;row < TTY_DISPHEIGHT;row++) {
			int ch = ttyDisplay[col+row*TTY_DISPWIDTH];
			if ((ch & 0x80) != 0 && GFXTimer()/300%2 == 0) {
				GFXCharacter(GRID(col+19,row+4),ch & 0x7F,GRIDSIZE,0x000,0xF80);
			} else {
				GFXCharacter(GRID(col+19,row+4),ch & 0x7F,GRIDSIZE,0xF80,0x000);
			}
		}
}	

