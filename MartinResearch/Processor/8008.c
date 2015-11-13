// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		8008.C
//		Purpose:	8008 Emulator optimised for size.
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "8008.h"

// *******************************************************************************************************************************
//														CPU Registers etc.
// *******************************************************************************************************************************

static BYTE8  registers[8];															// A,B,C,D,E,H,L and placeholder for (HL)
static BYTE8  carry,halt,pszValue;													// CPU Flags
static WORD16 pctrArray[8];															// 8 level rotating PC counters
static BYTE8  pctrIndex;															// Index into PC
static WORD16 cycles;																// CPU Cycles executed

#define HLVALUE	((registers[5] << 8) | registers[6])								// Composite value of HL 
#define CYCLES(n) cycles += (n) 													// Bump Cycle counter

// *******************************************************************************************************************************
//														  Reset the 8008
// *******************************************************************************************************************************

void CPUReset(void) {
	carry = 0;halt = 0;pszValue = 0;pctrIndex = 0;cycles = 0;						// Clear all single registers, sets P and Z.
	for (BYTE8 i = 0;i < 8;i++) registers[i] = pctrArray[i] = 0;					// Clear all registers and pctr stack.
}

// *******************************************************************************************************************************
//											Force an 8008 Interrupt (jams RST in)
// *******************************************************************************************************************************

void  CPUInterrupt(BYTE8 vector) {
	halt = 0;																		// Restart processor if stopped.
	pctrIndex = (pctrIndex + 1) & 7;												// Push PC on stack by rotating it.
	pctrArray[pctrIndex] = vector;													// Put vector into the program counter
}

// *******************************************************************************************************************************
//														Fetch a single byte
// *******************************************************************************************************************************

static BYTE8 _CPUFetch(void) {
	BYTE8 c = CPUXRead(pctrArray[pctrIndex]);										// Read a byte
	pctrArray[pctrIndex] = (pctrArray[pctrIndex] + 1) & 0x3FFF;						// Increment address in 14 bits.
	return c;																		// Return the byte read.
}

// *******************************************************************************************************************************
//															Execute rotates 
// *******************************************************************************************************************************

#define ACC 	registers[0] 														// Macro for readability.

static void _CPURotate(BYTE8 opcode) {
	WORD16 temp;
	switch(opcode) {
		case 0x02:																	// $02 RLC : Rotate A Left into Carry.
			carry = (ACC >> 7) & 0x01;												// C is bit 7 of A
			ACC = ((ACC << 1) & 0xFE) | carry;										// Shift A left and or carry in A:0
			break;
		case 0x0A:																	// $0A RRC : Rotate A Right into Carry.
			carry = ACC & 1;														// C is bit 0 of A.
			ACC = ((ACC >> 1) & 0x7F) | (carry << 7);								// Shift A right and put carry in A:7
			break;
		case 0x12:																	// $12 RAL : 9 bit Rotate Left A:Carry
			temp = (ACC << 1) | carry;												// A7 ... A0 C
			ACC = temp & 0xFF;														// A6..A0 C to ACC
			carry = (temp >> 8);													// old A7 to Carry.
			break;
		case 0x1A:																	// $1A RAR : 9 bit Rotate Right A:Carry
			temp = (carry << 8) | ACC;												// C A7 ... A0 in temp16
			ACC = (temp >> 1);														// C A7..A1 to ACC
			carry = temp & 1;														// Old A0 to carry.
			break;
	}
}

// *******************************************************************************************************************************
//														Simple Parity Test. 
// *******************************************************************************************************************************

static BYTE8 _CPUIsParityEven(BYTE8 n) {
	BYTE8 count = 0;																// Count of '1' bits
	while (n != 0) {																// Work through the bits while there are 1's
		if ((n & 1) != 0) count++;													// Bump count if bit set
		n = (n >> 1);																// Shift n right.
	}
	return ((count & 1) == 0);														// If count of 1 bits is even,it's even.
}

// *******************************************************************************************************************************
//													Evaluate the condition bits.
// *******************************************************************************************************************************

static BYTE8 _CPUCondition(BYTE8 opcode) {
	BYTE8 pass = 0;
	switch ((opcode >> 3) & 0x03) {													// Extract out C4C3 bits.
		case 0x00:	pass = (carry != 0);break;										// 00 : carry set.
		case 0x01:	pass = (pszValue == 0);break;									// 01 : A is zero.
		case 0x02: 	pass = ((pszValue & 0x80) != 0);break;							// 10 : A is signed.
		case 0x03:	pass = _CPUIsParityEven(pszValue);break;						// 11 : A is even parity.
	}
	if ((opcode & 0x20) == 0) pass = (pass == 0);									// If bit 5 is '0' it is a condition false.
	return (pass);
}

// *******************************************************************************************************************************
//															ALU Calculations
// *******************************************************************************************************************************

static void _CPUArithmeticLogic(BYTE8 opcode,BYTE8 operand) {
	WORD16 result = registers[0];
	opcode = (opcode >> 3) & 0x07;													// extract bits 3,4,5
	switch(opcode) {																// test on bits 3,4,5.
		case 0x00:																	// xx00 0xxx	Add without carry in.
			carry = 0;																// Clear carry, fall through.
		case 0x01:																	// xx00 1xxx 	Add with carry in.
			result += (operand + carry);											// calculate result.
			carry = (result >> 8) & 1;												// Get carry out.
			break;
		case 0x02:																	// xx01 0xxx  	Subtract without borrow
			carry = 0;																// Clear carry, fall through.
		case 0x03:																	// xx01 1xxx 	Subtract with borrow in.
		case 0x07:																	// xx11 1xxx	Compare (no borrow in)
			result -= (operand + carry);											// calculate result.
			carry = (result >> 8) & 1;												// Get carry out.
			break;												
		case 0x04:																	// xx10 0xxx	And
			result &= operand;														// All these logical ops. clear the carry
			carry = 0;
			break;
		case 0x05:																	// xx10 1xxx	Xor
			result ^= operand;
			carry = 0;
			break;
		case 0x06:																	// xx11 0xxx	Or
			result |= operand;
			carry = 0;
			break;
	}
	pszValue = (result & 0xFF);														// Save the answer for P,S,Z flags.
	if (opcode != 0x07) registers[0] = pszValue;									// Update Acc if it is not Compare.
}

// *******************************************************************************************************************************
//									Execute a single CPU instruction, if not halted
// *******************************************************************************************************************************

BYTE8 CPUExecute(WORD16 frameSize) {
	BYTE8 opcode = _CPUFetch();														// Fetch opcode.
	BYTE8 temp8,step;
	WORD16 temp16;
	if (halt != 0) {																// Are we in HALT ?
		cycles = 0;																	// If so clear cycle counter
		return 1;																	// And return frame ended, nothing happens till INT.
	}
	if (opcode < 2 || opcode == 0xFF) { 											// $00 $01 and $FF are all HALT
		halt = 1;
		cycles = frameSize;															// Force the end of the frame.
	} else { 																		// Execute instruction.
		temp8 = (((opcode & 0xC0) >> 3) | (opcode & 0x07)); 						// Map 76543210 to ...76210
		switch(temp8) {

			case 0x00:																// 0x00 is 00xx x000 INR (not A or M)
			case 0x01:																// 0x01 is 00xx x001 DCR (not A or M)
				temp8 = (opcode & 0x38) >> 3;										// Extract register number.
				step = ((opcode & 1) != 0) ? 0xFF:0x01;								// step $FF for DCR, $01 for INR
				registers[temp8] = pszValue = (registers[temp8] + step) & 0xFF; 	// Adjust it, setting PSZ but leaving C.
				CYCLES(5);				
				break;

			case 0x02:																// 0x02 is 00xx x010 RAL,RAR,RLC,RRC
				_CPURotate(opcode);													// Perform the appropriate rotate
				CYCLES(5);
				break;

			case 0x03:																// 0x03 and 0x07 are 00xx x?11 which are RETs.
			case 0x07:
				if (temp8 == 0x03) {												// 0x03 are conditional RETs
					if (_CPUCondition(opcode) == 0) temp8 = 0;						// If condition fails, clear temp8 e.g. no ret
				} 
				if (temp8 != 0) pctrIndex = (pctrIndex-1) & 7;						// which is unconditional return which is done 
				CYCLES(4);															// by rotating the stack.
				break;

			case 0x04:																// 0x04 is 00xx x100 which are ALU Commands, immediate
				temp8 = _CPUFetch();												// Fetch the operand
				_CPUArithmeticLogic(opcode,temp8);									// And perform it.
				CYCLES(8);
				break;

			case 0x05:																// 0x05 is 00xx x101 are RST Commands.
				pctrIndex = (pctrIndex + 1) & 7;									// Push PC on stack by rotating it.
				pctrArray[pctrIndex] = opcode & 0x38;								// Put new address extracted from command.
				CYCLES(5);
				break;

			case 0x06:																// 0x06 is 00xx x110 which are MVI Commands
				temp8 = (opcode & 0x38) >> 3;										// Extract register number.
				registers[temp8] = _CPUFetch();										// Set the register to the immediate byte.
				if (temp8 == 0x7) CPUXWrite(HLVALUE,registers[temp8]);				// If it was MVI M,xx write it to memory.
				CYCLES(8);
				break;

			case 0x08: case 0x0A: case 0x0C: case 0x0E:								// 0x08-0x0E are 01xx x??0 - jump and call.
				temp16 = _CPUFetch();												// Fetch CALL/JMP address and make it 14 bit.
				temp16 = (temp16 + (_CPUFetch() << 8)) & 0x3FFF;
				if ((opcode & 0x08) == 0) {											// Is it conditional (bit 3 is clear)
					if (_CPUCondition(opcode) == 0) temp8 = 0;						// If the condition is not satisfied, cancel it.
				}
				if (temp8 != 0) {													// Condition running ?
					if ((opcode & 0x02) != 0) {										// If bit 1 is set it is CALL so ....
						pctrIndex = (pctrIndex + 1) & 7;							// Push PC on stack by rotating it.
					}
					pctrArray[pctrIndex] = temp16; 									// Update PC with 2 byte parameter.
				}
				CYCLES(10);
				break;

			case 0x09: case 0x0B: case 0x0D: case 0x0F:								// 0x09-0x0F are 01xx x??1 which are the I/O Commands.
				registers[0] = CPUXInputOutput((opcode & 0x3E) >> 1,registers[0]);	// Call the I/O handler with the in/out reference.
				CYCLES(7);
				break;

			case 0x10: case 0x11: case 0x12: case 0x13:								// 0x10-0x17 are 10ff fsss which are memory/register
			case 0x14: case 0x15: case 0x16: case 0x17:								// arithmetic and logic operations.
				if ((opcode & 0x07) == 0x07) registers[7] = CPUXRead(HLVALUE);		// if using (HL) make sure it is updated.
				_CPUArithmeticLogic(opcode,registers[opcode & 7]);					// Do it with supplied value.
				CYCLES(6);
				break;

			case 0x18: case 0x19: case 0x1A: case 0x1B:								// 0x18-0x1F are 11ss s??? reg/mem move.
			case 0x1C: case 0x1D: case 0x1E: case 0x1F:
				temp8 = opcode & 7;													// which register is the source ?
				if (temp8 == 7) registers[7] = CPUXRead(HLVALUE);					// if it is M, read HL in.
				registers[opcode & 7] = registers[temp8];							// Copy into destination.
				if ((opcode & 7) == 7) CPUXWrite(HLVALUE,registers[7]);				// If moving to Memory copy last register out.
				CYCLES(6);
				break;
		}
	}
	temp8 = 0;																		// Return value.
	if (cycles >= frameSize) {														// Have we done a frame.
		cycles = cycles - frameSize;												// Reduce cycle count by frame size.
		temp8 = 1;																	// Return 1 (frame over.)
	}	
	return temp8;
}

// *******************************************************************************************************************************
//									Read the CPU Status (optionally compiled to save space)
// *******************************************************************************************************************************

#ifdef READS_CPU_STATUS																// Does it actually need this ?
																					// Only really required for a debugger.

typedef struct _CPU_Status {														// This structure returns the flags etc.
	int PC,returnStack[7],stackDepth;												// Current PC, Stack and Stack Depth.
} CPUSTATUS;

static CPUSTATUS s;																	// Stores the status.

CPUSTATUS *CPUGetStatus(void) {
	s.A = registers[0];s.B = registers[1];s.C = registers[2];s.D = registers[3];	// 8 bit registers
	s.E = registers[4];s.H = registers[5];s.L = registers[6];

	s.zFlag = (pszValue == 0);s.cFlag = (carry != 0);								// Flags
	s.nFlag = ((pszValue & 0x80) != 0);s.pFlag = _CPUIsParityEven(registers[0]);
	s.hFlag = (halt != 0);

	s.PC = pctrArray[pctrIndex];s.stackDepth = 0;									// Save current PC and clear the stack out.
	for (int i = 0;i < 7;i++) s.stack[i] = 0;

	int i = pctrIndex;																// Copy return addresses.
	while (i > 0) {																	// While stuff on stack
		i--;																		// Go to previous call.
		s.stack[s.stackDepth++] = pctrArray[i];										// Copy it into the stack
	}
	s.HL = (s.H << 8) | s.L;														// Support values.
	s.M = CPUXRead(s.HL & 0x3FFF);													
	s.cycles = cycles;
	return s;
}			
#endif
