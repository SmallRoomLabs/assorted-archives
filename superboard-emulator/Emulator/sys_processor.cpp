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

// *******************************************************************************************************************************
//														   Timing
// *******************************************************************************************************************************

#define CYCLE_RATE 		(1000000)													// Cycles per second
#define FRAME_RATE		(60)														// Frames per second (60)
#define CYCLES_PER_FRAME (CYCLE_RATE / FRAME_RATE)									// Cycles per frame (3,728)

// *******************************************************************************************************************************
//														CPU / Memory
// *******************************************************************************************************************************

#define CONSTTYPE static BYTE8 const 

static BYTE8 ramMemory[RAMSIZE];													// Memory at $0000 upwards
static BYTE8 videoRAM[1024];														// 1k Video RAM $D000-$D3FF

#include "__rom_basic.h"															// ROM Basic $A000-$BFFF
#include "__rom_monitor.h"															// 512 byte Monitor ROM $F800-$FFFF

static WORD16 cycles;

// *******************************************************************************************************************************
//											 Memory and I/O read and write macros.
// *******************************************************************************************************************************

#define READ(a) 	_Read(a)
#define WRITE(a,d)	_Write(a,d)

#define SETPC(a) 	pc = (a)
#define CYCLES(n) 	cycles += (n)
#define FETCH() 	_Read(pc++)

#define READWORD(a) (READ(a) | ((READ((a)+1) << 8)))
#define READWORD01(a) READWORD(a)
#define READ01(a) 	READ(a)
#define WRITE01(a,d) WRITE(a,d)
#define FETCHWORD()	{ temp16 = FETCH();temp16 |= (FETCH() << 8); }

static inline BYTE8 _Read(WORD16 address);
static inline void _Write(WORD16 address,BYTE8 data);

#include "__6502support.h"

static inline BYTE8 _Read(WORD16 address) {
	BYTE8 retVal;
	switch (address >> 12) {
		case 0x00:	retVal = ramMemory[address];break;
		case 0x0A:
		case 0x0B:	retVal = romBasic[address & 0x1FFF];break;
		case 0x0D:	retVal = videoRAM[address & 0x3FF];break;
		case 0x0F:	retVal = romMonitor[address & 0x7FF];break;
		default:	retVal = DEFAULT_BUS_VALUE;break;
	}
	return retVal;
}

static inline void _Write(WORD16 address,BYTE8 data) {
	switch (address >> 12) {
		case 0:		ramMemory[address] = data;break;
		case 0x0D:	videoRAM[address & 0x3FF] = data;break;
	}
}

// *******************************************************************************************************************************
//														Reset the CPU
// *******************************************************************************************************************************

void CPUReset(void) {
	resetProcessor();
}

// *******************************************************************************************************************************
//												Execute a single instruction
// *******************************************************************************************************************************

BYTE8 CPUExecuteInstruction(void) {

	BYTE8 opcode = FETCH();															// Fetch opcode.

	switch(opcode) {																// Execute it.
		#include "__6502opcodes.h"
	}
	if (cycles < CYCLES_PER_FRAME) return 0;										// Not completed a frame.
	cycles = cycles - CYCLES_PER_FRAME;												// Adjust this frame rate.
	return FRAME_RATE;																// Return frame rate.
}

#ifdef INCLUDE_DEBUGGING_SUPPORT

// *******************************************************************************************************************************
//		Execute chunk of code, to either of two break points or frame-out, return non-zero frame rate on frame, breakpoint 0
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 breakPoint1,WORD16 breakPoint2) { 
	do {
		BYTE8 r = CPUExecuteInstruction();											// Execute an instruction
		if (r != 0) return r; 														// Frame out.
	} while (pc != breakPoint1 && pc != breakPoint2);								// Stop on breakpoint.
	return 0; 
}

// *******************************************************************************************************************************
//									Return address of breakpoint for step-over, or 0 if N/A
// *******************************************************************************************************************************

WORD16 CPUGetStepOverBreakpoint(void) {
	BYTE8 opcode = CPUReadMemory(pc);												// Current opcode.
	if (opcode == 0x20) return (pc+3) & 0xFFFF;										// Step over JSR.
	return 0;																		// Do a normal single step
}

// *******************************************************************************************************************************
//												Read/Write Memory
// *******************************************************************************************************************************

BYTE8 CPUReadMemory(WORD16 address) {
	return READ(address);
}

void CPUWriteMemory(WORD16 address,BYTE8 data) {
	WRITE(address,data);
}

#endif

// *******************************************************************************************************************************
//											Retrieve a snapshot of the processor
// *******************************************************************************************************************************

#ifdef INCLUDE_DEBUGGING_SUPPORT

static CPUSTATUS st;																	// Status area

CPUSTATUS *CPUGetStatus(void) {
	st.a = a;st.x = x;st.y = y;st.sp = s;st.pc = pc;
	st.carry = carryFlag;st.interruptDisable = interruptDisableFlag;st.zero = (zValue == 0);
	st.decimal = decimalFlag;st.brk = breakFlag;st.overflow = overflowFlag;
	st.sign = (sValue & 0x80) != 0;st.status = constructFlagRegister();
	st.cycles = cycles;
	return &st;
}

#endif
