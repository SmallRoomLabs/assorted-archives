//*******************************************************************************************************
//*******************************************************************************************************
//
//      Name:       Core.C (6800)
//      Purpose:    CPU Core
//      Author:     Paul Robson
//      Date:       14th February 2014
//
//*******************************************************************************************************
//*******************************************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include "general.h"
#include "system.h"
#include "debugger.h"
#include "core.h"

#define RAMSIZE 			(4096)													// Amount of program RAM.

static void writeMemory(WORD16 addr,BYTE8 data);									// Forward definitions for R/W
static BYTE8 readMemory(WORD16 addr);
static void CPUWriteScreen(int x,int y,int ch);

static WORD16 cycleCount; 															// Cycle Count
static BYTE8 ramMemory[RAMSIZE];													// RAM Memory
static BYTE8 displayMemory[512];													// Display Memory.

#define CLOCKSPEED 			(666000L)												// Clock Speed (1.5us cycle time)
#define FRAMERATE 			(64L) 													// Frame rate (RTC tick rate)
#define CYCLESPERFRAME 		((WORD16)(CLOCKSPEED/FRAMERATE))						// CPU Cycles per frame.
#define INSTRUCTIONSPERSEC 	(CLOCKSPEED/3L) 										// Instructions per second, roughly.
#define INSTRPEREMUFRAME 	(INSTRUCTIONSPERSEC/30) 								// About 30 refreshes a second.

#define setProgramCounter(n) { pc = (n); }											// Anything involving PC change (except increments)

#define addCycles(n) 	cycleCount += (n)											// Counting cycles.

#define readByte(n) 	readMemory(n) 												// Mem Read/Write Macros
#define writeByte(n,d)  writeMemory(n,d)
#define readBytePage0(a) 	ramMemory[a] 											// Zero Page Operations.
#define writeBytePage0(a,d) ramMemory[a] = d

#define fetchByte() 	readMemory(pc++)	 										// 8 bit fetch


#include "6800_autocode.h"															// This is the system generated code 
#include "alu6800.h"																// 6800 ALU Functionality.
#include "rom_pds.h"

void CPUInitialise(int argc,char *argv[]) {

}

//*******************************************************************************************************
//										   Memory Read
//*******************************************************************************************************

static BYTE8 readMemory(WORD16 addr) {
	if (addr < RAMSIZE) return ramMemory[addr]; 									// RAM 		0000 upwards.
	if ((addr >> 9) == 0x70) return displayMemory[addr & 0x1FF];					// VRAM		E000-E1FF
	if (addr >= 0xF100) return _pdsROM[addr & 0xFFF];								// ROM 		F100-FFFF

	if (addr == 0xF041) return SYSIsKeyAvailable() ? 0x40 : 0x00; 					// PIA Flag F041
	if (addr == 0xF040) return SYSReadKeyboard();									// PIA Flag F041 Keyboard.
	return 0xFF; 																	// Unavailable.
}

//*******************************************************************************************************
//									Write to Memory / IO Devices
//*******************************************************************************************************

static void writeMemory(WORD16 addr,BYTE8 data) {
	if (addr < RAMSIZE) { 															// Write to RAM (0000 upwards)
		ramMemory[addr] = data;
	}
	if ((addr >> 9) == 0x70) { 														// Write to Display Memory (E000-E1FF)
		addr = addr & 0x1FF;														// Position in Display Memory
		if (displayMemory[addr] != data) {											// Has it changed ?
			displayMemory[addr] = data; 											// Update the screen memory.
			CPUWriteScreen(addr & 0x1F,addr >> 5,data);								// Update the physical screen.
		}
	}
}

//*******************************************************************************************************
//											Reset the Processor
//*******************************************************************************************************


void CPUReset(void) {
	WORD16 i;
	resetProcessor(); 																// Resets the CPU
	#ifdef DEBUGGABLE
	DBGSetCodeAddress(pc);
	#endif
}

//*******************************************************************************************************
//								Execute one or more instructions on the TMS1100
//*******************************************************************************************************

BYTE8 CPUExecute(BYTE8 singleStep) {
	WORD16 count = (singleStep != 0) ? 1 : INSTRPEREMUFRAME;						// How many to do ?
	WORD16 breakPoint = DBGGetCurrentBreakpoint();									// Get the current breakpoint.
	do {
		BYTE8 opcode = fetchByte();													// Fetch opcode.
		switch(opcode) {															// Execute operation code
			#include "6800_opcodes.h"
		}
		if (cycleCount > CYCLESPERFRAME) { 											// Cycle time out
			cycleCount = 0;															// Reset it.
			SYSSynchronise(FRAMERATE);
		}
	}
	while (--count > 0 && pc != breakPoint);										// Execute until breakpoint or tState counter high or once in S/S mode
	return (pc == breakPoint);														// Return true if at breakpoint.
}

#ifdef DEBUGGABLE

//*******************************************************************************************************
//										Return the Status of the Processor
//*******************************************************************************************************

static CPUSTATUS stat;

int _readMemory(int addr) { return readMemory(addr); }								// required because the functions are int/int.

CPUSTATUS *CPUGetStatus(CPUSTATUS *st) {
	if (st == NULL) st = &stat;
	st->a = a;st->b = b;st->ccr = getCCR();
	st->ix = ix;st->pc = pc;st->sp = sp;
	st->readCodeMemory = _readMemory;
	st->readDataMemory = _readMemory;
	return st;
}

//*******************************************************************************************************
//									Get Instruction Mnemonic and Length
//*******************************************************************************************************

#include "6800_mnemonics.h"

int CPUGetInstructionInfo(int address,char *buffer) {
	int opcode = readMemory(address); 												// Get the opcode
	char *p = _mnemonics[opcode];													// Get the mnemonic
	if (buffer != NULL) strcpy(buffer,p);											// Copy into buffer if required.
	int count = 1; 																	// Count number of '@' in opcode, +1
	while (*p != '\0') if (*p++ == '@') count++;
	return count;																	// Return size.
}

//*******************************************************************************************************
//									Get code and data address masks
//*******************************************************************************************************

int CPUGetCodeAddressMask(void) { return 0xFFFF; }
int CPUGetDataAddressMask(void) { return 0xFFFF; }

//*******************************************************************************************************
//											CPU Debug Renderer
//*******************************************************************************************************

void CPURenderDebug(SDL_Rect *rcDisplay,int codeAddress,int dataAddress) {
	rcDisplay->x = 58;rcDisplay->y = 2;rcDisplay->w = 40;rcDisplay->h = 50; 		// Set rectangle for screen in percentages.
	char *labels[] = { "A","B","IX","SP","PC","BK","CC",NULL };
	DBGUtilsLabels(16,0,labels,2);
	int n = 0;
	SYSWriteHex(19,n++,a,3,2);
	SYSWriteHex(19,n++,b,3,2);
	SYSWriteHex(19,n++,ix,3,4);
	SYSWriteHex(19,n++,sp,3,4);
	SYSWriteHex(19,n++,pc,3,4);
	SYSWriteHex(19,n++,DBGGetCurrentBreakpoint(),3,4);
	SYSWriteHex(19,n++,getCCR(),3,2);
	for (int i = 0;i < 6;i++) {
		SYSWriteCharacter(17+i,n+1,"HINZVB"[i],((getCCR() << i) & 0x20) ? 3 : 5);
	}
	DBGUtilsDisassemble(0,0,12,codeAddress,pc);
	DBGUtilsDisplay(1,14,23,dataAddress,8,2,TRUE,ix);
}

static void CPUWriteScreen(int x,int y,int ch);
static DisplaySurface mainDisplay;													// Surface for screen display.

//*******************************************************************************************************
//									Initialise resources for the display
//*******************************************************************************************************

void CPUInitialiseResources(void) {
	SYSCreateSurface(&mainDisplay,32*6,16*8);
	for (int x = 0;x < 32;x++)
		for (int y = 0;y < 16;y++)
			CPUWriteScreen(x,y,rand());
}

//*******************************************************************************************************
//										Get the Display Surface
//*******************************************************************************************************

DisplaySurface *CPUGetDisplaySurface(void) {
    return &mainDisplay;
}

static void CPUWriteScreen(int x,int y,int ch) {
	if (x < 0 || y < 0 || x >= 32 || y >= 16) return;
	SDL_Rect rcSrc,rcTgt;
	rcSrc.x = (ch & 0x3F) * 6;rcSrc.y = 0;rcSrc.w = 5;rcSrc.h = 7;
	rcTgt.x = x * 6;rcTgt.y = y * 8;rcTgt.w = 5;rcTgt.h = 7;
	if (ch & 0x80)
		SDL_FillRect(mainDisplay.surface,&rcTgt,SDL_MapRGB(mainDisplay.surface->format,0,165,225));
	else
		SDL_BlitSurface(SYSGetFontSurface()->surface,&rcSrc,mainDisplay.surface,&rcTgt);
}

//*******************************************************************************************************
//									Any special ASCII keyboard keys ?
//*******************************************************************************************************

int CPUScanCodeToASCIICheck(int scanCode) {
	return -1;
}

#endif
