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

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CRYSTAL_CLOCK 	(3500000L)													// Clock cycles per second (3.5Mhz)
#define CYCLE_RATE 		(CRYSTAL_CLOCK/2)											// Cycles per second (1.75Mhz)
#define FRAME_RATE		(50)														// Frames per second (50)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (35,000)

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

static BYTE8 AC,EX,SR;																// CPU Registers
static WORD16 P0,P1,P2,P3,cycles;		
static WORD16 doubleCyclesToWaste; 													// Number of double cycles to waste.

static BYTE8 expansionRAM[1024];													// 1k x 8 RAM (800-BFF)
static BYTE8 ioChipRAM[128];														// 1/8k x 8 RAM (F80-FFF)
static BYTE8 videoRAM[256];															// 256 x 8 (E00-FFF)

static BYTE8 MB,opcode,operand,temp8;												// Working storage
static WORD16 MA,temp16;

#ifdef ARDUINO
//TODO: Store ROM in Flash memory ?
#else
static BYTE8 romMemory[2048];														// Otherwise, use 2k memory
#endif 

// *******************************************************************************************************************************
//													Memory read and write macros.
// *******************************************************************************************************************************

#define READ() 		_Read()															// Too complex to macroize.
#define WRITE() 	_Write()

static void _Read(void) {
	MB = DEFAULT_BUS_VALUE;															// Whatever it floats to.

	switch((MA >> 8) & 0xF) {
		case 0x8: case 0x9: case 0xA: case 0xB:										// RAM ($800-$BFF)
			MB = expansionRAM[MA & 0x3FF];
			break;
		case 0xC:																	// Keyboard ($C00-$CFF)
			MB = KEYBOARDPORT(MA & 0xFF);
			break;
		case 0xE:																	// VDU RAM ($E00-$EFF)
			MB = videoRAM[MA & 0xFF];
			break;
		case 0xF:																	// 8154 RAM ($F80-$FFF)
			if ((MA & 0x80) != 0) {													// Only $80-$FF e.g. test bit 7.				
				MB = ioChipRAM[MA & 0x7F];
			}
			break;
		default:																	// ROM ($000-$7FF)
			MB = romMemory[MA & 0x7FF];
		}
}

static void _Write(void) {
	
	switch((MA >> 8) & 0xF) {
		case 0x8: case 0x9: case 0xA: case 0xB:										// RAM ($800-$BFF)
			expansionRAM[MA & 0x3FF] = MB;
			break;
		case 0xE:																	// VDU RAM ($E00-$EFF)
			temp16 = MA & 0xFF;														// Position in Video RAM
			VDUUPDATE(temp16,MB,videoRAM[temp16]);									// Tell video hardware we've updated.
			videoRAM[temp16] = MB;
			break;
		case 0xF:																	// 8154 RAM ($F80-$FFF)
			if ((MA & 0x80) != 0) {													// Only $80-$FF e.g. test bit 7.
				ioChipRAM[MA & 0x7F] = MB;
			}
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

#define SENSEA()		(HWIIsIntPressed())											// Keyboard Inputs.
#define SENSEB()		(0)

#define DELAYCPU(op,ac)	{}															// Instigate a CPU delay.

#define STATUSREG() 	((SR & 0xCF) | (SENSEA() ? 0x10:0) | (SENSEB() ? 0x10:0))	// Construct the actual SR value

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
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

static BYTE8 *pOpcode;																// Keep pointer to opcode speed up 2 byte
static BYTE8 _dummyMemory[] = { DEFAULT_BUS_VALUE,DEFAULT_BUS_VALUE };				// Dummy for non-code areas.

BYTE8 CPUExecuteInstruction(void) {

	P0 = (P0 + 1) & 0xFFFF;															// Bump P0.

	switch ((P0 >> 8) & 0xF) {
		case 0x0:case 0x1:case 0x2:case 0x3:case 0x4:case 0x5:case 0x6:case 0x7:
			pOpcode = &romMemory[P0 & 0x7FF];										// 000-7FF ROM
			break;
		case 0x8:case 0x9:case 0xA:case 0xB:										// 800-BFF Expansion RAM.
			pOpcode = &expansionRAM[P0 & 0x3FF];
			break;
		case 0xE:																	// E00-EFF Video RAM 
			pOpcode = &videoRAM[P0 & 0xFF];	
			break;
		case 0xF:																	// F80-FFF IO RAM
			pOpcode = _dummyMemory;											
			if ((P0 & 0x80) != 0) pOpcode = &ioChipRAM[P0 & 0x7F];					// Upper half of page *only*
			break;
		default:
			pOpcode = _dummyMemory;													// Pages C,D return dummy.
			break;
	}

	opcode = *pOpcode;																// Read opcode
	if ((opcode & 0x80) != 0) {														// Two byte instruction, fetch operand.
		P0 = (P0 + 1) & 0xFFFF;														// Bump P0.
		operand = *++pOpcode;														// Read the operand.
	}

	switch(opcode) {																// Execute it.
		#include "__opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
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

void DBGXLoad(char *file) {
	FILE *f = fopen(file,"rb");
	int n = 0;
	while (!feof(f)) romMemory[n++] = fgetc(f);
	fclose(f);	
}

#endif
