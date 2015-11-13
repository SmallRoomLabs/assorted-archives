// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_scelbi.c
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
#include "drivers.h"

#include "__8008mnemonics.h"														// 8008 new style mnemonics

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

static int xDisplay,yDisplay;														// Display position.
static BYTE8 displayMemory[20*4];													// Video Memory

// *******************************************************************************************************************************
//												Reset the 8008
// *******************************************************************************************************************************

void DBGXReset(void) {
	CPUReset();
}

// *******************************************************************************************************************************
//											Write to display memory
// *******************************************************************************************************************************

void DBGXWriteDisplay(BYTE8 x,BYTE8 y,BYTE8 ch) {
	if (x < 20) xDisplay = x;
	if (y < 4) yDisplay = y;
	displayMemory[xDisplay+yDisplay*20] = ch;
	if (++xDisplay == 20) {
		xDisplay = 0;yDisplay = (yDisplay + 1) % 4;
	}
}

// *******************************************************************************************************************************
//										  This renders the debug screen
// *******************************************************************************************************************************

//	s.status = status;s.interruptMode = interruptMode;								// Internal statuses.
//	s.singleStepMode = singleStepMode;s.cpuPhase = cpuPhase;

void DBGXRender(int *address,int showDisplay) {
	int n;
	char buffer[32];
	GFXSetCharacterSize(32,25);

	CPUSTATUS *s = CPUGetStatus();													// Get the CPU Status

	const char *labels[] = { "A","B","C","D","E","H","L","M","C","Z","S","P","H","HL","BP","CY",NULL };
	n = 0;
	while (labels[n] != NULL) {
		GFXString(GRID(15,n),labels[n],GRIDSIZE,DBGC_ADDRESS,-1);
		n++;
	}
	const char *labels2[] = { "ST","IM","SS","PH","AD","DA","TS","","PC","SK",NULL };
	n = 0;
	while (labels2[n] != NULL) {
		GFXString(GRID(25,n),labels2[n],GRIDSIZE,DBGC_ADDRESS,-1);
		n++;
	}

	n = address[1];																	// Dump memory.
	for (int row = 17;row < 25;row++) {
		GFXNumber(GRID(1,row),n & 0x3FFF,16,4,GRIDSIZE,DBGC_ADDRESS,-1);			// Head of line
		GFXCharacter(GRID(6,row),':',GRIDSIZE,DBGC_HIGHLIGHT,-1);
		for (int col = 0;col < 8;col++) {											// Data on line
			GFXNumber(GRID(8+col*3,row),CPURead(n & 0x3FFF),16,2,GRIDSIZE,DBGC_DATA,-1);
			n++;
		}
	}
																					// Output text labels.																					// Macros to simplify dump drawing.
	#define DD(value,width)	GFXNumber(GRID(18,n++),value,16,width,GRIDSIZE,DBGC_DATA,-1)
	#define DDC(value) GFXNumber(GRID(18,n++),value,16,1,GRIDSIZE,DBGC_DATA,-1)

	n = 0;																			// Draw the registers
	DD(s->a,2);DD(s->b,2);DD(s->c,2);DD(s->d,2);DD(s->e,2);DD(s->h,2);DD(s->l,2);DD(s->m,2);
	DDC(s->cFlag);DDC(s->zFlag);DDC(s->sFlag);DDC(s->pFlag);DDC(s->hFlag);			// Draw the flags
	DD(s->hl,4);DD(address[3],4);DD(s->cycles,4);									// The rest.

	n = 9;																			// PCTR
	GFXNumber(GRID(28,n),s->pc,16,4,GRIDSIZE,DBGC_DATA,-1);
	n++;
	GFXString(GRID(28,n),"----",GRIDSIZE,DBGC_DATA,-1);								// Translated stack.
	for (int i = 0;i < s->stackDepth;i++) 
		GFXNumber(GRID(28,n++),s->stack[i],16,4,GRIDSIZE,DBGC_DATA,-1);

	GFXNumber(GRID(28,0),s->status >> 1,16,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(30,0),s->status & 1,16,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,1),s->interruptMode & 1,16,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,2),s->singleStepMode & 1,16,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,3),s->cpuPhase,16,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,4),s->addressLamps,16,4,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,5),s->dataLamps,16,2,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,6),DRVReadToggleSwitches(),16,2,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(28,7),DRVReadToggleSwitches(),8,3,GRIDSIZE,DBGC_DATA,-1);

	//if (showCPU == 0) return;

	n = address[0]; 																// Dump code.
	for (int row = 0;row < 16;row++) {
		int isPC = (n & 0x3FFF) == (s->pc);											// Check for breakpoint and being at PC
		int isBrk = ((n & 0x3FFF) == address[3]);
		GFXNumber(GRID(0,row),n & 0x3FFF,16,4,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_ADDRESS,isBrk ? 0xF00 : -1);
		strcpy(buffer,__mnemonics[CPURead(n & 0x3FFF)]);							// Get mnemonic
		n++;
		if (buffer[strlen(buffer)-2] == '@') {										// Replace @1 @2 with 1/2 byte operands
			switch(buffer[strlen(buffer)-1]) {
				case '1':
					sprintf(buffer+strlen(buffer)-2,"%02x",CPURead((n) & 0x3FFF));
					n++;
					break;
				case '2':
					sprintf(buffer+strlen(buffer)-2,"%02x%02x",(CPURead((n+1) & 0x3FFF)) & 0x3F,CPURead((n) & 0x3FFF));
					n += 2;
					break;
			}
		}
		GFXString(GRID(5,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_DATA,-1);
	}

	if (showDisplay == 0) return;

	SDL_Rect rc,rc2;
	rc.w = 20 * 8 * 4;rc.h = 4 * 10 * 4;
	rc.x = (WIN_WIDTH-rc.w)/2;rc.y = WIN_HEIGHT-64-rc.h;
	rc2 = rc;rc.x--;rc.y--;rc.w += 2;rc.h += 2;GFXRectangle(&rc,0xFFF);
	GFXRectangle(&rc2,0x000);
	for (int x = 0;x < 20;x++)
		for (int y = 0;y < 4;y++) {
			GFXCharacter(rc2.x+x*8*4,rc2.y+y*10*4+4,displayMemory[x+y*20],4,0xF80,-1);
		}
}	
