// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		sys_processor.c
//		Purpose:	Processor Emulation.
//		Created:	21st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "drivers.h"

// *******************************************************************************************************************************
//															Timing
// *******************************************************************************************************************************
	
#define CYCLES_PER_SECOND	(500000/2)												// Clock at 500Khz, 2 Clocks per T-State	
#define FRAME_RATE			(60)													// Frame rate
#define CYCLES_PER_FRAME	(CYCLES_PER_SECOND/FRAME_RATE)							// T-States per second.

// *******************************************************************************************************************************
//													   Processor status
// *******************************************************************************************************************************

static BYTE8 	A,B,C,D,E,H,L;														// Registers
static BYTE8 	PSZValue,Carry,HaltFlag;											// Flags
#define PCTR 	PC[PCIndex]															// Program counter

static BYTE8  	MB,interruptRequested,PCIndex,cpuPhase,								// Internal states & registers
									interruptMode,singleStepMode,status,opcode;
static WORD16 	PC[8],MA,Cycles; 	

static WORD16 	temp16;																// Work register

static WORD16 	addressLamps;														// What's on the lamps.
static BYTE8 	dataLamps;

// *******************************************************************************************************************************
//														Main Memory
// *******************************************************************************************************************************

static BYTE8 ramMemory[RAMSIZE];													// System RAM.							

// *******************************************************************************************************************************
//														Other keys
// *******************************************************************************************************************************

static BYTE8 interruptKey, stepKey, runKey;

// *******************************************************************************************************************************
//											    Memory read and write macros.
// *******************************************************************************************************************************

#define READ() 		MB = ((MA < RAMSIZE) ? ramMemory[MA] : DEFAULT_BUS_VALUE);
#define WRITE() 	if (MA < RAMSIZE) ramMemory[MA] = MB;

// *******************************************************************************************************************************
//											Other macros that link to functions.
// *******************************************************************************************************************************

#define READPORT() 	{}
#define WRITEPORT()	{}

#define FETCH()		_CPUFetch()
#define CYCLES(n) 	Cycles += (n)
#define PUSH() 		PCIndex = (PCIndex + 1) & 7
#define PULL() 		PCIndex = (PCIndex - 1) & 7
#define HL() 		((((WORD16)H) << 8) | L)
#define STATUS(s) 	status = (s)
#define DISPLAY(a,d) { addressLamps = (a);dataLamps = (d); }

#define FETCH2() 	{ FETCH();temp16 = ((((WORD16)MB) << 8) | MB) & 0x3FFF; }

// *******************************************************************************************************************************
//												Fetch an instruction byte
// *******************************************************************************************************************************

static inline void _CPUFetch() {
	if (interruptMode == 0) {														// No jam
		MA = PCTR;																	// Fetch from PC
		PCTR = (PCTR + 1) & 0x3FFF;
		READ();
	} else {																		// IJAM on.
		MA = 0;
		MB = DRVReadToggleSwitches();												// Get the toggles to figure out what..
		//printf("JAM:%x\n",MB);
	}
}
// *******************************************************************************************************************************
//												Parity Checker (true if even)
// *******************************************************************************************************************************

static BYTE8 _CPUParityEven(BYTE8 n) {
	BYTE8 p = 0;
	while (n != 0) {																// While bits set left
		if ((n & 1) != 0) p ^= 1;													// Toggle if bit set
		n = n >> 1;																	// Next bit
	}
	return (p == 0);																// Return true if even.
}

// *******************************************************************************************************************************
//													  Binary addition
// *******************************************************************************************************************************

static inline BYTE8 _CPUAdd(BYTE8 n) {
	temp16 = A + n + Carry; 														// Calculate result
	Carry = temp16 >> 8;															// Calculate carry
	return temp16 & 0xFF;															// Return result
}

// *******************************************************************************************************************************
//													  Binary subtraction
// *******************************************************************************************************************************

static inline BYTE8 _CPUSubtract(BYTE8 n) {
	temp16 = A - n - Carry; 														// Calculate result
	Carry = temp16 >> 8;															// Calculate carry
	return temp16 & 0xFF;															// Return result.
}

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	A = B= C = D = E = H = L = 0;													// Clear registers
	HaltFlag = Carry = Cycles = PCIndex = PSZValue = cpuPhase = status = 0;			// Clear flags & internals
	interruptRequested = 0;interruptMode = 0;singleStepMode = 0;
	for (BYTE8 n = 0;n < 8;n++) PC[n] = 0;											// Zero stack for clarity.
	DRVReset();																		// Reset drivers.
}

// *******************************************************************************************************************************
//											Check to see if a key has been pressed
// *******************************************************************************************************************************

static BYTE8 _CPUHasKeyBeenPressed(BYTE8 *state, BYTE8 keyID) {
	BYTE8 newState = DRVIsPushButtonPressed(keyID);									// Get state
	BYTE8 isPressed = 0;
	if (newState != *state) {														// State changed
		if (newState != 0) isPressed = 1;											// New state is down, set pressed
		*state = newState;															// Update current state
		//if (isPressed) printf("%c pressed.\n",keyID);
	}
	return isPressed;
}

// *******************************************************************************************************************************
//													 Execute a single phase.
// *******************************************************************************************************************************

BYTE8 CPUExecuteSinglePhase(void) {

	BYTE8 canExecute;																// true if we can clock a CPU cycle.

	if (interruptRequested != 0 && cpuPhase == 0) {									// Interrupt requested at start of instruction.
		interruptRequested = 0;														// We no longer have a request.
		HaltFlag = 0;																// We are no longer halted.
		interruptMode = 1;															// We are in interrupt mode (from toggles)
		singleStepMode = 1;															// We are in single step mode.
	}

	canExecute = (HaltFlag == 0);													// Can execute command if Halt not set

	if (singleStepMode != 0) {														// Single step mode ?
		if (_CPUHasKeyBeenPressed(&stepKey,DRVKEY_STEP) == 0) canExecute = 0;		// Can't execute if Step not pressed.
	}

	if (canExecute != 0) {															// If we can run an instruction this time.
		if (cpuPhase == 0) {														// Phase 0, fetch opcode.
			FETCH();opcode = MB;													
			cpuPhase = 1;															// Go to phase 1.
			STATUS(0);DISPLAY(MA,MB);												// Display new instruction.
		}
		switch(opcode) {
			#include "__8008opcodes.h"
			default:
				cpuPhase = 0;break;
		}
		if (cpuPhase == 0) {														// At end of instruction
			interruptMode = 0;														// Interrupt mode off
			//singleStepMode = 0;														// Step mode off.
		}
	}

	if (canExecute != 0 && Cycles < CYCLES_PER_FRAME) return 0;						// Frame in progress, return 0.

	if (Cycles >= CYCLES_PER_FRAME) Cycles -= CYCLES_PER_FRAME;
	if (HaltFlag != 0) {															// If halt flag set.
		Cycles = 0;																	// Fix up for HALT.
	}
	if (_CPUHasKeyBeenPressed(&interruptKey,DRVKEY_INTERRUPT)) {					// Interrupt requested
		interruptRequested = 1;
	}
	if (_CPUHasKeyBeenPressed(&runKey,DRVKEY_RUN)) {								// Run requested
		singleStepMode = 0;
	}
	DRVRefreshPanel(addressLamps,dataLamps,status,									// Update Panel.
							interruptMode,HaltFlag,singleStepMode == 0);			
	DRVEndFrame();																	// Update hardware where required.
	return FRAME_RATE;																// Return the frame rate for sync speed.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//											Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {
	BYTE8 frameRate;
	while (1) {
		frameRate = CPUExecuteSinglePhase();										// Execute a phase
		if (frameRate != 0) return frameRate;
	}
	return frameRate;																// When we can exit.
}

// *******************************************************************************************************************************
//										 Get the step over breakpoint value
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPURead(PCTR);													// Read opcode.
	if ((opcode & 0xC7) == 0x07) return ((PCTR+1) & 0x3FFF);						// RST xx
	if ((opcode & 0xC3) == 0x42) return ((PCTR+3) & 0x3FFF);						// CALL xxxx (various calls)
	return 0xFFFF;
}

// *******************************************************************************************************************************
//										Run continuously till breakpoints / Halt.
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 break1,WORD16 break2) {
	BYTE8 rate = 0;
	while(1) {
		rate = CPUExecuteInstruction();												// Execute one instruction phase.
		if (rate != 0) {															// If end of frame, return rate.
			return rate;													
		}
		if (PCTR == break1 || PCTR == break2) return 0;
	} 																				// Until hit a breakpoint or HLT.
}

// *******************************************************************************************************************************
//												Load a binary file into RAM
// *******************************************************************************************************************************

#include <stdio.h>

void CPULoadBinary(const char *fileName) {
	FILE *f = fopen(fileName,"rb");
	fread(ramMemory,1,RAMSIZE,f);
	fclose(f);
}

// *******************************************************************************************************************************
//												Gets a pointer to RAM memory
// *******************************************************************************************************************************
	
BYTE8 CPURead(WORD16 address) {
	WORD16 _MA = MA;BYTE8 _MB = MB;BYTE8 result;
	MA = address;READ();result = MB;
	MA = _MA;MB = _MB;
	return result;
}

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

static CPUSTATUS s;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	int i;
	s.a = A;s.b = B;s.c = C;s.d = D;s.e = E;s.h = H;s.l = L;						// 8 bit registers
	s.zFlag = (PSZValue == 0);s.cFlag = (Carry != 0);s.hFlag = (HaltFlag != 0);		// Flags
	s.pFlag = _CPUParityEven(PSZValue);s.sFlag = ((PSZValue & 0x80) != 0);
	s.cycles = Cycles;																// Number of cycles.
	for (i = 0;i < 8;i++) s.stack[i] = 0;											// Clear stack.
	s.pc = PCTR;																	// Save PC.
	i = PCIndex-1;s.stackDepth = 0;													// Copy stack.
	while (i >= 0) s.stack[s.stackDepth++] = PC[i--];
	s.hl = (s.h << 8) | s.l;s.m = CPURead(s.hl & 0x3FFF);							// Helpers
	s.status = status;s.interruptMode = interruptMode;								// Internal statuses.
	s.singleStepMode = singleStepMode;s.cpuPhase = cpuPhase;
	s.addressLamps = addressLamps;s.dataLamps = dataLamps;							// Lamp displays.
	return &s;
}
#endif
