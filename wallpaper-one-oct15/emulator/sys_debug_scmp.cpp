// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		debug_scmp.c
//		Purpose:	Debugger Code (System Dependent)
//		Created:	22nd October 2015
//		Author:		Paul Robson (paul@robsons->org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gfx.h"
#include "sys_processor.h"
#include "debugger.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

static const char *__mnemonics[] = {
	#include "__scmp_mnemonics.h"
};

static const BYTE8 __bitmapFont[] = {
	#include "__font8x8.h"
};

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

static const char *labels[] = { "A","E","S","P0","P1","P2","P3","CL","BP","CY", NULL };

void DBGXRender(int *address,int showDisplay) {
	int n = 0;
	char buffer[32],buffer2[4];
	CPUSTATUS *s = CPUGetStatus();
	GFXSetCharacterSize(28,20);
	DBGVerticalLabel(19,0,labels,DBGC_ADDRESS,-1);									// Draw the labels for the register

	#define DN(v,w) GFXNumber(GRID(22,n++),v,16,w,GRIDSIZE,DBGC_DATA,-1)			// Helper macro

	//s->a = HWIReadKeyboard();

	n = 0;
	DN(s->a,2);DN(s->e,2);DN(s->s,2);												// Dump Registers etc.
	DN(s->p0,4);DN(s->p1,4);DN(s->p2,4);DN(s->p3,4);		
	DN((s->s >> 7) & 1,1);DN(address[3],4);DN(s->cycles,4);

	int a = address[1];																// Dump Memory.
	for (int row = 11;row < 20;row++) {
		GFXNumber(GRID(0,row),a,16,4,GRIDSIZE,DBGC_ADDRESS,-1);
		GFXCharacter(GRID(4,row),':',GRIDSIZE,DBGC_HIGHLIGHT,-1);
		for (int col = 0;col < 8;col++) {
			GFXNumber(GRID(5+col*3,row),CPUReadMemory(a),16,2,GRIDSIZE,DBGC_DATA,-1);
			a = (a + 1) & 0xFFFF;
		}		
	}
	int p = address[0];																// Dump program code. 
	int opc,opr;
	for (int row = 0;row < 10;row++) {
		int isPC = (p == ((s->p0+1) & 0xFFFF));										// Tests.
		int isBrk = (p == address[3]);
		GFXNumber(GRID(2,row),p,16,4,GRIDSIZE,isPC ? DBGC_HIGHLIGHT:DBGC_ADDRESS,	// Display address / highlight / breakpoint
																	isBrk ? 0xF00 : -1);
		opc = CPUReadMemory(p);p = (p + 1) & 0xFFFF;								// Read opcode.
		if ((opc & 0x80) != 0) {
			opr = CPUReadMemory(p);p = (p + 1) & 0xFFFF;							// Read operand.
		}
		strcpy(buffer,__mnemonics[opc]);												// Set the mnemonic.
		if (buffer[0] == '\0') sprintf(buffer,"db %02x",opc);						// Make up one if required.

		char *ph = strchr(buffer,'#');												// Insert operand
		if (ph != NULL) {
			sprintf(buffer2,"%02x",opr);
			*ph++ = buffer2[0];
			*ph++ = buffer2[1];
		}
					
		GFXString(GRID(7,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT:DBGC_DATA,-1);	// Print the mnemonic
	}

	if (showDisplay == 0) return;

	int xSize = 5;
	int ySize = 6;

	SDL_Rect rc;
	rc.w = 8 * 16 * xSize;										// 8 x 8 font, 16 x 8 grid
	rc.h = 8 * 8 * ySize;			
	rc.x = WIN_WIDTH/2-rc.w/2;rc.y = WIN_HEIGHT-64-rc.h;
	SDL_Rect rc2 = rc;
	rc2.x -= 10;rc2.y -= 10;rc2.w += 20;rc2.h += 20;
	GFXRectangle(&rc2,0xFFF);
	rc2.x += 2;rc2.y += 2;rc2.w -= 4;rc2.h -= 4;
	GFXRectangle(&rc2,0x13F);
	SDL_Rect rpix;rpix.w = xSize-1;rpix.h = ySize-1;
	for (int x = 0;x < 16;x++) {
		for (int y = 0;y < 8;y++) {
			int ch = HWIGetVideoMemory()[x+y*16];
			if (ch != 0 && ch != 64 && ch != 240) {
				for (int y1 = 0;y1 < 8;y1++) {
					rpix.x = x * 8 * xSize + rc.x;
					rpix.y = y * 8 * ySize + y1 * ySize + rc.y;
					int bits = __bitmapFont[ch * 8 + y1];
					while (bits != 0) {
						if (bits & 0x80) GFXRectangle(&rpix,0xFFF);
						rpix.x += xSize;
						bits = (bits << 1) & 0xFF;
					}
				}
			}
		}
	}

}	