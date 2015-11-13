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

#include "gfx.h"
#include "sys_processor.h"
#include "debugger.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

#define TWIDTH			(28)
#define THEIGHT			(7)

static char ttyBuffer[TWIDTH*THEIGHT];												// Text in buffer

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

#define CHAR(x,y,c,co) GFXCharacter(GRID(x,y),c,GRIDSIZE,co,-1)
#define NUM(x,y,n,co) GFXNumber(GRID(x,y),n,8,4,GRIDSIZE,co,-1)

#include "__mnemonics_old.h"

void DBGXRender(int *address) {
	CPUSTATUS *s = CPUGetStatus();
	char buffer[16],num[5];
	GFXSetCharacterSize(44,19);
	CHAR(16,0,'A',DBGC_ADDRESS);CHAR(17,0,':',DBGC_HIGHLIGHT);
	NUM(18,0,s->a,DBGC_DATA);
	CHAR(23,0,'P',DBGC_ADDRESS);CHAR(24,0,':',DBGC_HIGHLIGHT);
	NUM(25,0,s->p,DBGC_DATA);
	CHAR(23,1,'C',DBGC_ADDRESS);CHAR(24,1,':',DBGC_HIGHLIGHT);
	NUM(25,1,s->cycles,DBGC_DATA);
	CHAR(30,0,'Z',DBGC_ADDRESS);CHAR(31,0,':',DBGC_HIGHLIGHT);
	NUM(32,0,s->z,DBGC_DATA);
	CHAR(37,0,'B',DBGC_ADDRESS);CHAR(38,0,'P',DBGC_ADDRESS);CHAR(39,0,':',DBGC_HIGHLIGHT);
	NUM(40,0,address[3] & 07777,DBGC_DATA);
	CHAR(16,1,'H',DBGC_ADDRESS);CHAR(17,1,':',DBGC_HIGHLIGHT);
	NUM(18,1,s->halt ? 1 : 0,DBGC_DATA);
	CHAR(37,1,'S',DBGC_ADDRESS);CHAR(38,1,'W',DBGC_ADDRESS);CHAR(39,1,':',DBGC_HIGHLIGHT);
	NUM(40,1,address[2] & 07777,DBGC_DATA);

	for (int row = 0;row < 10;row++) {
		int addr = (address[0] + row) & 07777;
		int isPC = (addr == s->p);
		int isBrk = (addr == address[3]);
		int cmd = CPURead(addr);
		NUM(0,row,addr,isPC ? DBGC_HIGHLIGHT:DBGC_DATA);
		strcpy(buffer,_mnemonics[cmd >> 6]);
		cmd = cmd & 077;
		char *p = strstr(buffer,"%c");
		if (p != NULL) {
			sprintf(num,"%02o",cmd);
			strncpy(p,num,strlen(num));
		}
		GFXString(GRID(5,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT:DBGC_DATA,isBrk ? 0xF00:-1);
	}

	for (int row = 11;row < 19;row++) {
		int addr = ((row - 11) * 8 + address[1]) & 07777;
		NUM(0,row,addr,DBGC_ADDRESS);CHAR(4,row,':',DBGC_HIGHLIGHT);
		for (int col = 0;col < 8;col++) {
			NUM(col*5+5,row,CPURead(addr+col),DBGC_DATA);
		}
	}

	for (int col = 0;col < TWIDTH;col++)
		for (int row = 0;row < THEIGHT;row++) {
			int ch = ttyBuffer[col+row*TWIDTH];
			if (ch < ' ') ch = '.';
			GFXCharacter(GRID(col+16,row+3),ch,GRIDSIZE,0x0F0,0x000);
		}
}	

// *******************************************************************************************************************************
//												Write to teletype
// *******************************************************************************************************************************

void DBGXWriteTTYDisplay(int x,int y,int ch) {
	if (x >= 0 && y >= 0 && x <= TWIDTH && y <= THEIGHT) {
		ttyBuffer[x+y*TWIDTH] = (ch & 0x7F);
	}
}