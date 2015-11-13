// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_mark8.cpp
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

#include "__8008mnemonics.h"														// 8008 new style mnemonics
#include "mcmfont.h"																// MCM6571 7x9 font.

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

static BYTE8 videoMemory[256];														// 256 x 8 RAM inside TVT

// *******************************************************************************************************************************
//												Reset the 8008
// *******************************************************************************************************************************

void DBGXReset(void) {
	CPUReset();
}

// *******************************************************************************************************************************
//												Write to video ram
// *******************************************************************************************************************************

void DBGXWriteVideoMemory(BYTE8 address,BYTE8 data) {
	videoMemory[address] = data;
}

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

void DBGXRender(int *address,int showDisplay) {
	int n;
	char buffer[32];
	GFXSetCharacterSize(32,25);

//	int k = IFReadKeyboard();
//	GFXNumber(GRID(26,2),k,16,2,GRIDSIZE,0xF80,0xF00);
//	if (k >= ' ') GFXCharacter(GRID(26,3),k,GRIDSIZE,0xF80,0xF00);

	CPUSTATUS *s = CPUGetStatus();													// Get the CPU Status

	const char *labels[] = { "A","B","C","D","E","H","L","M","C","Z","S","P","H","HL","BP","CY",NULL };
	n = 0;
	while (labels[n] != NULL) {
		GFXString(GRID(15,n),labels[n],GRIDSIZE,DBGC_ADDRESS,-1);
		n++;
	}
	const char *labels2[] = { "PC","ST",NULL };
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

	n = 0;																			// PCTR
	GFXNumber(GRID(28,n),s->pc,16,4,GRIDSIZE,DBGC_DATA,-1);
	n++;
	GFXString(GRID(28,n),"----",GRIDSIZE,DBGC_DATA,-1);								// Translated stack.
	for (int i = 0;i < s->stackDepth;i++) 
		GFXNumber(GRID(28,n++),s->stack[i],16,4,GRIDSIZE,DBGC_DATA,-1);

	//if (showCPU == 0) return;

	n = address[0]; 																// Dump code.
	for (int row = 0;row < 16;row++) {
		int isPC = (n & 0x3FFF) == (s->pc);											// Check for breakpoint and being at PC
		int isBrk = ((n & 0x3FFF) == address[3]);
		GFXNumber(GRID(0,row),n & 0x3FFF,16,4,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_ADDRESS,isBrk ? 0xF00 : -1);
		strcpy(buffer,_mnemonics[CPURead(n & 0x3FFF)]);							// Get mnemonic
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
	SDL_Rect rc,rcc;
	int scale = 3;
	rcc.w = 8*scale;rcc.h = 14*scale;
	rc.w = 32 * rcc.w;rc.h = 8 * rcc.h;
	rc.w += 2;rc.h += 2;
	rc.x = (WIN_WIDTH - rc.w) / 2;
	rc.y = (WIN_HEIGHT - 32 - rc.h);
	GFXRectangle(&rc,0xFFF);
	rc.w -= 2;rc.h -= 2;rc.x++;rc.y++;
	GFXRectangle(&rc,0x000);

	rcc.w = rcc.h = scale;
	for (int x = 0;x < 32;x++) {
		for (int y = 0;y < 8;y++) {
			int ch = videoMemory[(x + y * 32)] & 0x7F;
			if (ch != ' ') {
				for (int y1 = 0;y1 < 9;y1++) {
					rcc.x = x * 8 * scale + rc.x;
					rcc.y = (y*14+y1+2) * scale + rc.y;
					int bits = _mcm6571font[ch * 9+y1];
					for (int x1 = 0;x1 < 7;x1++) {
						if (bits & (0x80 >> x1)) GFXRectangle(&rcc,0xF80);
						rcc.x += rcc.w;
					}
				}
			}
		}
	}
}	
