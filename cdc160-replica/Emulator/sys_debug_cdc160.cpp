// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_cdc160.c
//		Purpose:	Debugger Code (System Dependent)
//		Created:	3rd September 2015
//		Author:		Paul Robson (paul@robsons->org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gfx.h"
#include "sys_processor.h"
#include "debugger.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

#define TWIDTH			(28)
#define THEIGHT			(8)

static char ttyBuffer[TWIDTH*THEIGHT];												// Text in buffer
static int  ttyX,ttyY;																// Teletype position
static int  ttxInitialised = 0;														// Non zero if ttx initialised.

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

#define STR(x,y,c,co) GFXString(GRID(x,y),c,GRIDSIZE,co,-1)
#define NUM(x,y,n,co) GFXNumber(GRID(x,y),n,8,4,GRIDSIZE,co,-1)

#define NEWMNEMONICS
#include "__CDC160Mnemonics.h"

void DBGXRender(int *address) {
	CPUSTATUS *s = CPUGetStatus();
	char buffer[16];
	GFXSetCharacterSize(44,23);

	if (ttxInitialised == 0) {														// Initialise TTY Device.
		ttyX = ttyY = 0;ttxInitialised = 1;
		for (int i = 0;i < TWIDTH*THEIGHT;i++) ttyBuffer[i] = ' ';
	}

	STR(16,0,"AC",DBGC_ADDRESS);STR(18,0,":",DBGC_HIGHLIGHT);
	NUM(19,0,s->a,DBGC_DATA);
	STR(24,0,"PC",DBGC_ADDRESS);STR(26,0,":",DBGC_HIGHLIGHT);
	NUM(27,0,s->p,DBGC_DATA);
	STR(32,0,"ZR",DBGC_ADDRESS);STR(34,0,":",DBGC_HIGHLIGHT);
	NUM(35,0,s->z,DBGC_DATA);

	STR(16,1,"CY",DBGC_ADDRESS);STR(18,1,":",DBGC_HIGHLIGHT);
	NUM(19,1,s->cycles,DBGC_DATA);
	STR(24,1,"BP",DBGC_ADDRESS);STR(26,1,":",DBGC_HIGHLIGHT);
	NUM(27,1,address[3] & 07777,DBGC_DATA);
	STR(32,1,"HA",DBGC_ADDRESS);STR(34,1,":",DBGC_HIGHLIGHT);
	GFXNumber(GRID(38,1),s->halt,8,1,GRIDSIZE,DBGC_DATA,-1);
	//NUM(35,1,s->halt ? 1 : 0,DBGC_DATA);

	for (int row = 0;row < 11;row++) {
		int addr = (address[0] + row) & 07777;
		int isPC = (addr == s->p);
		int isBrk = (addr == address[3]);
		int cmd = CPURead(addr);
		NUM(0,row,addr,isPC ? DBGC_HIGHLIGHT:DBGC_DATA);
		strcpy(buffer,_mnemonics[cmd >> 6]);
		#ifdef NEWMNEMONICS
			char *p = buffer + strlen(buffer)-2;							
			if (*p == '%') {
				switch (toupper(*(p+1))) {
					case 'D':
						sprintf(p,"%02o",cmd & 077);
						break;
					case 'F':
						sprintf(p,"%04o",(addr+(cmd & 077)) & 07777);
						break;
					case 'B':
						sprintf(p,"%04o",(addr-(cmd & 077)) & 07777);
						break;
				}
			}
		#else
			cmd = cmd & 077;
		strlenprintf(buffer+strlen(buffer)," %02o",cmd);
		#endif
		GFXString(GRID(5,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT:DBGC_DATA,isBrk ? 0xF00:-1);
	}

	for (int row = 12;row < 23;row++) {
		int addr = ((row - 12) * 8 + address[1]) & 07777;
		NUM(0,row,addr,DBGC_ADDRESS);STR(4,row,":",DBGC_HIGHLIGHT);
		for (int col = 0;col < 8;col++) {
			NUM(col*5+5,row,CPURead(addr+col),DBGC_DATA);
		}
	}

	for (int col = 0;col < TWIDTH;col++)
		for (int row = 0;row < THEIGHT;row++) {
			int ch = ttyBuffer[col+row*TWIDTH];
			if (col == ttyX && row == ttyY && GFXTimer()/300%2 == 0) {
				GFXCharacter(GRID(col+16,row+3),ch,GRIDSIZE,0x000,0xF80);
			} else {
				GFXCharacter(GRID(col+16,row+3),ch,GRIDSIZE,0xF80,0x000);
			}
		}
}	

// *******************************************************************************************************************************
//										Write to teletype (ASCII codes)
// *******************************************************************************************************************************

void DBGXWriteTTYDisplay(BYTE8 character) {
}

