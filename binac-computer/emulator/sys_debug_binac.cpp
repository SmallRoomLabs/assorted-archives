// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_debug_cdc160.cpp
//		Purpose:	Debugger Code (System Dependent)
//		Created:	12th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gfx.h"
#include "sys_processor.h"
#include "sys_debug_system.h"
#include "debugger.h"

#define DBGC_ADDRESS 	(0x0F0)														// Colour scheme.
#define DBGC_DATA 		(0x0FF)														// (Background is in main.c)
#define DBGC_HIGHLIGHT 	(0xFF0)

// *******************************************************************************************************************************
//												Dump Word in Octal and FP equivalent
// *******************************************************************************************************************************

static void _DBGXWord(int x,int y,LONG32 w) {
	GFXNumber(GRID(x,y),(w >> 15) & 0177777,8,6,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(x+7,y),w & 077777,8,5,GRIDSIZE,DBGC_DATA,-1);
	if ((w & SIGN31) != 0) {
		w = ((w ^ MASK31) + 1) & MASK31;
		GFXCharacter(GRID(x+13,y), '-',GRIDSIZE,DBGC_DATA,-1);
	}
	double f = BINMToFloat(w);
	char szBuffer[32];
	sprintf(szBuffer,"%G",f);
	strcpy(szBuffer,szBuffer+1);
	if (fabs(f) < 0.0001 && f != 0) {
		sprintf(szBuffer,"%.4E",f);
		int n = atoi(szBuffer+strlen(szBuffer)-2);
		sprintf(szBuffer+4,"e-%d",n);
	}
	if (f == 0) strcpy(szBuffer,"0");
	GFXString(GRID(x+14,y),szBuffer,GRIDSIZE,DBGC_DATA,-1);
}

// *******************************************************************************************************************************
//											This renders the debug screen
// *******************************************************************************************************************************

#define STR(x,y,c,co) GFXString(GRID(x,y),c,GRIDSIZE,co,-1)
#define NUM(x,y,n,co) GFXNumber(GRID(x,y),n,8,4,GRIDSIZE,co,-1)

static const char *oldMnemonics[] = { 	"","STOP","F","D", "C","A","","",		// Old Mnemonics
										"M","K","L","H","T","S","","",
										"U","","+","-","BP","SKIP","","",
										"","","","","","","",""
									};

static const char *newMnemonics[] = { 	"","STOP","ADDL","DIV", "STC","ADD","","",	 // New Mnemonics
										"MUL","TALC","TAL","STA","JMC","SUB","","",
										"JMP","","ASHL","ASHR","BRK","NOP","","",
										"","","","","","","",""
									};

void DBGXRender(int *address,int showDisplay) {

	GFXSetCharacterSize(42,23);

	CPUSTATUS *s = CPUGetStatus();													// Get the CPU Status

	STR(15,0,"ACC",DBGC_ADDRESS);STR(19,0,":",DBGC_HIGHLIGHT);
	STR(15,1,"L",DBGC_ADDRESS);STR(19,1,":",DBGC_HIGHLIGHT);
	STR(15,2,"MPC",DBGC_ADDRESS);STR(19,2,":",DBGC_HIGHLIGHT);
	STR(15,3,"CYC",DBGC_ADDRESS);STR(19,3,":",DBGC_HIGHLIGHT);
	STR(15,4,"BRK",DBGC_ADDRESS);STR(19,4,":",DBGC_HIGHLIGHT);
	STR(27,3,"HLT",DBGC_ADDRESS);STR(30,3,":",DBGC_HIGHLIGHT);

	_DBGXWord(20,0,s->acc);
	_DBGXWord(20,1,s->l);

	GFXNumber(GRID(20,2),s->mpc,8,4,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(20,3),s->cycles,8,4,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(25,2),s->mpcHalf,8,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(34,3),s->halt ? 1 : 0,8,1,GRIDSIZE,DBGC_DATA,-1);
	GFXNumber(GRID(20,4),address[3],8,4,GRIDSIZE,DBGC_DATA,-1);

	for (int row = 6;row < 23;row++) {
		int addr = ((row - 6) + address[1]) & MEMMASK;
		GFXNumber(GRID(15,row),addr,8,4,GRIDSIZE,DBGC_ADDRESS,-1);
		_DBGXWord(20,row,CPURead(addr));
		STR(19,row,":",DBGC_HIGHLIGHT);
	}

	for (int row = 0;row < 23;row++) {
		char buffer[32];
		int addr = (address[0] + row / 2) & MEMMASK;
		int part = row & 1;
		int isPC = (addr == s->mpc) && (part == s->mpcHalf);
		NUM(0,row,addr,isPC ? DBGC_HIGHLIGHT:DBGC_ADDRESS);
		int opcode = CPURead(addr);
		if (part == 0) opcode = opcode >> 15;
		opcode = opcode & 037777;
		int order = opcode >> 9;
		strcpy(buffer,oldMnemonics[order]);
		strcpy(buffer,newMnemonics[order]);
		if (buffer[0] == '\0') strcpy(buffer,"-");
		if (order == 03 || order == 04 || order == 05 || order == 010 || order == 013 || order == 014 ||
			order == 015 || order == 020) {
				sprintf(buffer+strlen(buffer)," %03o",opcode & 0777);
		}
		strlwr(buffer);
		GFXString(GRID(6,row),buffer,GRIDSIZE,isPC ? DBGC_HIGHLIGHT : DBGC_DATA,
											(addr == address[3] && part == 0) ? 0xF00:-1);
	}
	
}	
