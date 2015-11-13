// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		processor.c
//		Purpose:	Processor Emulation.
//		Created:	2nd September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "sys_debug_system.h"
#include "hardware.h"
#ifdef ARDUINO
#include <avr/pgmspace.h>
#endif

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CRYSTAL_CLOCK 	(1000000L)													// Clock cycles per second (1Mhz)
#define CYCLE_RATE 		(CRYSTAL_CLOCK/2)											// Cycles per second (500khz)
#define FRAME_RATE		(50)														// Frames per second (50)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8 AC,EX,SR;																// CPU Registers
static WORD16 P0,P1,P2,P3,cycles;		
static WORD16 doubleCyclesToWaste; 													// Number of double cycles to waste.

static BYTE8 MB,opcode,operand,temp8;												// Working storage
static WORD16 MA,temp16;

static BYTE8 ramMemory[1024];														// 1k Memory (x800-xBFF)

#ifdef WINDOWS 																		// The Rom Memory is an array of ROMTYPE
#define ROMTYPE BYTE8 																// Under something with lots of SRAM
#else
#define ROMTYPE const PROGMEM BYTE8													// Arduino, we use Flash Memory to store it
#define INFLASH
#endif

#include "__testcode.h"																// 2k ROM Memory, const char* or PROGMEM

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															// Too complex to macroize.
#define WRITE() 	_Write()

static inline void _Read(void) {
	MB = DEFAULT_BUS_VALUE;															// Whatever it floats to.

	switch((MA >> 10) & 0x3) {
		case 0x00:																	// ROM ($x000-$x7FF)
		case 0x01:
			#ifndef INFLASH
			MB = romMemory[MA & 0x7FF];
			#else
			MB = pgm_read_byte(romMemory+(MA & 0x7FF));
			#endif
			break;
		case 0x02:																	// RAM ($x800-$xBFF)
			MB = ramMemory[MA & 0x3FF];
			break;
		case 0x03:																	// Input are ($xC00-$xFFF)
			MB = ((MA & 1) != 0) ? HWIReadKeyboard() : HWIReadToggles();			// A0 = 0 Toggles, A0 = 1 ASCII Keyboard
			break;
		}
}

static inline void _Write(void) {
	
	switch((MA >> 10) & 0x3) {
		case 0x00:																	// ROM ($x000-$x7FF)
		case 0x01:
			HWIWriteLEDDisplay(MB);													// Writing to the ROM shows on LEDs.
			break;
		case 0x02:																	// RAM ($x800-$xBFF)
			ramMemory[MA & 0x3FF] = MB;
			break;
		case 0x03:																	// Video RAM (x$C00-x$FFF)
			HWIWriteVideoMemory(MA & 0x1F,(MA >> 5) & 0x1F,MB & 1); 				// This is write only
			break;
	}
}

// *******************************************************************************************************************************
//															Other macros
// *******************************************************************************************************************************

#define BINARYADD()		_BinaryAdd()												// Addition functions.
#define DECIMALADD()	_DecimalAdd()

#define CHECKEX() 		if (operand == 0x80) operand = EX;							// If operand $80, use EX as offset
#define PREINDEX(Px) 	if ((operand & 0x80) != 0) Px += (operand) - 256;			// If operand bit 7 one, add to Pn before EAC
#define EAC(Px)			MA = Px+(((operand & 0x80)==0) ? operand:(0xFF00|operand));	// Sign extend operand add to Pn -> MA
#define POSTINDEX(Px)	if ((operand & 0x80) == 0) Px += (operand);					// If operand bit 7 zero, add to Pn after EAC
#define CYCLES(n)		cycles += (n)												// Add to cycles

#define SENSEA()		(HWIReadControlKey(KEY_A))									// Button inputs.
#define SENSEB()		(HWIReadControlKey(KEY_B))

#define DELAYCPU(op,ac)	_StartDelay(op,ac)											// Instigate a CPU delay.

#define STATUSREG() 	((SR & 0xCF) | (SENSEA() ? 0x10:0) | (SENSEB() ? 0x20:0))	// Construct the actual SR value

#define UPDATEFLAGS(n) 	HWISetFrequency(freqTable[(n) & 7])							// Flags drive sound err. chip

static const WORD16 freqTable[8] = { 0,209,383,592,668,877,1051,0 };				// VCO 555 pitches.

// *******************************************************************************************************************************
//														  Arithmetic
// *******************************************************************************************************************************

static void _BinaryAdd(void) {
	temp16 = AC + MB + ((SR >> 7) & 1);												// Calculate result into temp16
	SR = (SR & 0x3F) | ((temp16 & 0x100) >> 1);										// Put carry flag back in.
	if ((AC & 0x80) == (MB & 0x80)) {												// Sign of inputs the same
		if ((AC & 0x80) != (temp16 & 0x80)) SR |= 0x40;								// If sign of result different set OV
	}
	AC = temp16 & 0xFF;																// Copy to AC
}

static void _DecimalAdd(void) {
	temp8 = (AC & 0xF) + (MB & 0xF) + ((SR > 7) & 1);								// Calculate Lower digit
	temp16 = (AC & 0xF0) + (MB & 0xF0);												// Calculate Upper digit
	SR = SR & 0x7F;																	// Clear Carry/Link.
	if (temp8 > 9) {																// Propagate from low to high (half carry)
		temp8 = temp8 - 10;															// Adjust lower digit	
		temp16 = temp16 + 0x10;														// Add to upper digit
	}
	if (temp16 > 0x90) {															// Carry out ?
		temp16 = temp16 - 0xA0;														// Adjust upper digit
		SR = SR | 0x80;																// Set carry flag.
	}
	AC = (temp8 | temp16) & 0xFF;													// Calculate result put in AC.
}
// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	AC = EX = SR = 0;																// Reset 8 bit registers
	P0 = P1 = P2 = P3 = 0;															// Reset Ptr registers
	cycles = 0;																		// Reset cycle count.
	doubleCyclesToWaste = 0;														// Reset cycle waste counter.
	HWIReset();																		// Reset hardware
}

// *******************************************************************************************************************************
//												  Start a DLY instruction
// *******************************************************************************************************************************

static void _StartDelay(BYTE8 operand,BYTE8 acc) {
	doubleCyclesToWaste = operand * 256;											// We aren't that worried, near enough :)

	WORD16 doubleCyclesRemaining = (CYCLES_PER_FRAME-cycles)/2;						// How many 2-cycles are left.
	if (doubleCyclesRemaining > doubleCyclesToWaste) {								// Can we do it this frame ?
		cycles = cycles + doubleCyclesToWaste * 2; 									// If so, bump the cycle count.
		doubleCyclesToWaste = 0;													// and nothing left to throw.
	} else {
		doubleCyclesToWaste -= doubleCyclesRemaining;								// What we use this frame.
		cycles = CYCLES_PER_FRAME;													// End this frame.
	}
}
// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {

	if (doubleCyclesToWaste > 0) {													// Time to waste due to DLY ?
		if (doubleCyclesToWaste > CYCLES_PER_FRAME/2) {								// Won't expire this time.
			doubleCyclesToWaste -= CYCLES_PER_FRAME / 2;
			HWIEndFrameUpdate();													// Call the end of frame code.
			return FRAME_RATE;
		}
		cycles = cycles + doubleCyclesToWaste * 2;									// Use up this many cycles
		doubleCyclesToWaste = 0;													// None left to waste.
	}

	MA = P0 = (P0 + 1) & 0xFFFF;													// Bump P0.
	READ();opcode = MB;																// Read opcode
	if ((opcode & 0x80) != 0) {														// Two byte instruction, fetch operand.
		MA = P0 = (P0 + 1) & 0xFFFF;												// Bump P0.
		READ();operand = MB;														// Read the operand.
	}
	
	switch(opcode) {																// Execute it.
		#include "__opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	if (HWIReadControlKey(KEY_RESET) != 0) CPUReset(); 								// Reset key.
	HWIEndFrameUpdate();															// Call the end of frame code.
	return FRAME_RATE;																// Return frame rate.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	breakPoint1 = (breakPoint1-1) & 0xFFFF;											// Fix to work with SC/MP preincrement
	breakPoint2 = (breakPoint2-1) & 0xFFFF;
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (P0 != breakPoint1 && P0 != breakPoint2);								// Stop on breakpoint.
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPUReadMemory((P0+1) & 0xFFFF);									// Read next opcode.
	if (opcode >= 0x3D && opcode <= 0x3F) {											// If XPPC P1,P2 or P3 (SC/MP function calls)
		return (P0+2) & 0xFFFF;														// Return the address after that.
	}
	return 0;																		// Do a normal single step
}

// *******************************************************************************************************************************
//												Read/Write Memory
// *******************************************************************************************************************************

BYTE8 CPUReadMemory(WORD16 address) {
	BYTE8 _return,_MB = MB;WORD16 _MA = MA;
	MA = address;READ();_return = MB;
	MA = _MA;MB = _MB;
	return _return;
}

void CPUWriteMemory(WORD16 address,BYTE8 data) {
	BYTE8 _MB = MB;WORD16 _MA = MA;
	MA = address;MB = data;WRITE();
	MA = _MA;MB = _MB;
}

#endif

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	s.a = AC;s.e = EX;s.s = STATUSREG();
	s.p0 = P0;s.p1 = P1;s.p2 = P2;s.p3 = P3;
	s.cycles = cycles;
	return &s;
}

#include <stdio.h>

void DBGXLoad(const char *file) {
	FILE *f = fopen(file,"rb");
	int n = 0;
	while (!feof(f)) romMemory[n++] = fgetc(f);
	fclose(f);	
}

#endif
