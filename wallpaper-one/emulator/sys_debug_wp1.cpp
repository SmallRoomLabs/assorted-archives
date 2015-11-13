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
#include "hardware.h"

#include "__mnemonics.h"															// SC/MP Instruction Set.

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

static BYTE8 videoMemory[1024];														// 32 x 32 pixel memory
static BYTE8 displayLEDs = 0;

void DBGXWriteVDUMemory(int x,int y,int isOn) {
	videoMemory[x+y*32] = (BYTE8)(isOn != 0);
}

void DBGXWriteLEDDisplay(int data) {
	displayLEDs = data & 0xFF;
}

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

static const char *labels[] = { "A","E","S","P0","P1","P2","P3","CL","BP","CY", NULL };

void DBGXRender(int *address,int showDisplay) {
	int n = 0;
	char buffer[32],buffer2[4];
	CPUSTATUS *s = CPUGetStatus();
	GFXSetCharacterSize(28,16);
	DBGVerticalLabel(19,0,labels,DBGC_ADDRESS,-1);									// Draw the labels for the register

	#define DN(v,w) GFXNumber(GRID(22,n++),v,16,w,GRIDSIZE,DBGC_DATA,-1)			// Helper macro

	//s->a = HWIReadKeyboard();

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

	if (showDisplay == 0) return;

	SDL_Rect rc,rc2,rc3;rc.w = WIN_WIDTH * 8 / 10;rc.h = WIN_HEIGHT / 4;
	rc.x = (WIN_WIDTH-rc.w)/2;rc.y = WIN_HEIGHT * 2/3;
	GFXRectangle(&rc,0xFFF);rc.w -= 2;rc.h -= 2;rc.x++;rc.y++;
	GFXRectangle(&rc,0x000);
	int border = 10;
	int on;
	rc.w -= border * 2;rc.h -= border * 2;rc.x += border;rc.y += border;
	for (int b = 0;b < 8;b++) {
			border = 10;
			rc2.w = rc.w / 8;rc2.h = rc.h;rc2.x = rc.x + b * rc2.w;rc2.y = rc.y;
			rc2.w -= border * 2;rc2.h -= border * 2;rc2.x += border;rc2.y += border;
			rc2.h = rc2.h / 2;
			rc3 = rc2;rc3.y = rc2.y + rc2.h;
			GFXRectangle(&rc2,0xFFF);
			rc2.w -= 2;rc2.h -= 2;rc2.x++;rc2.y++;
			on = displayLEDs & (0x80 >> b);
			GFXRectangle(&rc2,(on != 0) ? 0xF00 : 0x400);
			border = 15;
			rc3.w -= border * 2;rc3.h -= border * 2;rc3.x += border;rc3.y += border;
			GFXRectangle(&rc3,0xFFF);
			rc3.w -= 2;rc3.h -= 2;rc3.x++;rc3.y++;
			GFXRectangle(&rc3,0x444);
			border = rc3.w / 5;
			rc3.w -= border * 2;rc3.h = rc3.h * 3 / 2;rc3.x += border;rc3.y += border;
			if ((HWIReadToggles() & (0x80 >> b)) == 0) rc3.y = rc3.y - rc3.h * 2 / 3;
			GFXRectangle(&rc3,0xFFF);
			rc3.w -= 2;rc3.h -= 2;rc3.x++;rc3.y++;
			GFXRectangle(&rc3,0xFF0);
	}

	rc.h = (WIN_HEIGHT * 3 / 5 / 32) * 32;rc.w = (WIN_WIDTH / 2 / 32) * 32;
	rc.w += 2;rc.h += 2;
	rc.x = (WIN_WIDTH - rc.w) / 2;rc.y = (WIN_HEIGHT * 2 / 3 - rc.h) / 2;GFXRectangle(&rc,0xFFF);
	rc.w -= 2;rc.h -= 2;rc.x++;rc.y++;GFXRectangle(&rc,0x000);
	border = 10;rc.w -= border * 2;rc.h -= border * 2;rc.x += border;rc.y += border;

	rc2.w = rc.w / 32;rc2.h = rc.h / 32;
	for (int x = 0;x < 32;x++) {
		rc2.x = rc.x + rc2.w * x;
		rc2.y = rc.y;
		for (int y = 0;y < 32;y++) {
			if (videoMemory[x+y*32] != 0) GFXRectangle(&rc2,0xF80);
			rc2.y += rc2.h;
		}
	}
}	