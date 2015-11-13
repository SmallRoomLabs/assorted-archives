// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		debug_scmp.c
//		Purpose:	Debugger Code (System Dependent)
//		Created:	3rd September 2015
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

#include "__mnemonics.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

static void DGBXDrawCharacter(int x,int y,int ch);
static BYTE8 vduMemory[256];														// Current vdu memory

// *******************************************************************************************************************************
//												  Write to VDU Memory
// *******************************************************************************************************************************

void DBGXWriteVDUMemory(int address,int data) {
	vduMemory[address] = data;
}

// *******************************************************************************************************************************
//												Render the main display
// *******************************************************************************************************************************

void DBGXRenderDisplay(void) {
	SDL_Rect rc;
	rc.x = 0;rc.y = 0;rc.w = WIN_WIDTH;rc.h = WIN_HEIGHT;
	GFXRectangle(&rc,0);	
	for (int x = 0;x < 32;x++)
		for (int y = 0;y < 8;y++) {
			DGBXDrawCharacter(x,y,vduMemory[x+y*32]);
		}
}

// *******************************************************************************************************************************
//												Draw a single character
// *******************************************************************************************************************************

#include "__largefont.h"

static void DGBXDrawCharacter(int x,int y,int ch) {
	int size = 4;
	SDL_Rect rc;
	rc.w = rc.h = size;
	for (int xc = 0;xc < 8;xc++) {
		rc.x = (x * 8 + xc) * size + (WIN_WIDTH - 32 * 8 * size)/2;
		rc.y = y * 13 * size + (WIN_HEIGHT-8*13*size)/2;
		for (int yc = -2;yc < 11;yc++) {
			int b = _font8678[(ch & 0x3F) * 9 + yc];
			if (yc < 0 || yc >= 9) b = 0;
			if ((ch & 0x80) != 0) b = b ^ 0xFF;	
			if ((b & (128 >> xc)) != 0) GFXRectangle(&rc,0xFFF);
			rc.y += rc.h;
		}
	}
}

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

static char *labels[] = { "A","E","S","P0","P1","P2","P3","CL","BP","CY", NULL };

void DBGXRender(int *address) {
	int n = 0;
	char buffer[32],buffer2[4];
	CPUSTATUS *s = CPUGetStatus();
	GFXSetCharacterSize(28,16);
	DBGVerticalLabel(19,0,labels,DBGC_ADDRESS,-1);									// Draw the labels for the register

	#define DN(v,w) GFXNumber(GRID(22,n++),v,16,w,GRIDSIZE,DBGC_DATA,-1)			// Helper macro

	n = 0;
	DN(s->a,2);DN(s->e,2);DN(s->s,2);												// Dump Registers etc.
	DN(s->p0,4);DN(s->p1,4);DN(s->p2,4);DN(s->p3,4);		
	DN((s->s >> 7) & 1,1);DN(address[3],4);DN(s->cycles,4);

	int a = address[1];																// Dump Memory.
	for (int row = 11;row < 16;row++) {
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
		strcpy(buffer,_mnemonics[opc]);												// Set the mnemonic.
		if (buffer[0] == '\0') sprintf(buffer,"db %02x",opc);						// Make up one if required.

		char *ph = strchr(buffer,'#');												// Insert operand
		if (ph != NULL) {
			sprintf(buffer2,"%02x",opr);
			*ph++ = buffer2[0];
			*ph++ = buffer2[1];
		}
					
		GFXString(GRID(7,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT:DBGC_DATA,-1);	// Print the mnemonic
	}

}	