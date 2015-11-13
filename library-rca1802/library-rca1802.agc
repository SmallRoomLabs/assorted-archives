/// @name 	library-rca1802
/// @author Paul Scott Robson 
/// @email  paulscottrobson@gmail.com 
/// @license MIT
/// @prefix RCA
/// @provides rca1802
/// @requires
/// @version 0.1 
/// @created 10-Mar-15
/// @updated 10-Mar-15
/// @module

// These are the registers. These are globals for speed, plus I think it unlikely that anyone is going to want
// to run a pair of RCA1802s in the same program.

global _RCA_D as integer 														// 8 bit D register (Acc)
global _RCA_DF as integer 														// 1 bit DF register (Carry)
global _RCA_R as integer[16]													// 16 bit R registers (16 off)
global _RCA_P as integer 														// 4 bit P register (PCTR)
global _RCA_X as integer 														// 4 bit X register (Index)
global _RCA_T as integer 														// 8 bit T register
global _RCA_IE as integer 														// 1 bit IE register (Interrupt enable)
global _RCA_Q as integer 														// 1 bit Q flag.

/// Structure which is used to return the status of the RCA1802.

type RCAStatus
	D as integer
	DF as integer
	R as integer[16]
	P as integer
	X as integer
	T as integer
	IE as integer
endtype

#constant MEMORYSIZE	4096 													// Memory size.
#constant _RCAMEMMASK 	0x0FFF 													// Mask to apply when reading/writing.

#constant _RCAMASK16 	0xFFFF													// 16 bit mask
#constant _RCAMASK8 	0x00FF													// 8 bit mask
#constant _RCA_RCAMASKUPPER 	0xFF00 											// Upper 8 bit mask.

																				// Constants used in hardware access.
#constant RCA_IOEFLAG	0x10 													// 0x1n read EFLAG n
#constant RCA_IOWRITE 	0x20 													// 0x2n write line n
#constant RCA_IOREAD 	0x30 													// 0x3n read line n 
#constant RCA_IOWRITEQ 	0x40 													// 0x40 Q line write

global _RCAMemory as integer[MEMORYSIZE]										// 1802 Program Memory.

///	Reset the RCA1802 to its initial state.
// see 3-21 and 3-22 of specification.

function RCAReset()
	_RCA_Q = 0																	// Clear Q  
	_RCA_IE = 1																	// Set IE 	
	_RCA_X = 0																	// Clear X
	_RCA_P = 0																	// Clear P
	_RCA_R[0] = 0 																// Clear R(0)
	RCAAccessHardware(RCA_IOWRITEQ,0) 											// Update the hardware line Q, set to zero.
	
	i as integer:h as integer
	for i = 0 to MEMORYSIZE-1
		_RCAMemory[i] = random(0,255)
	next i
endfunction	

/// Read from memory. Not used in emulator but provides consistent interface.
/// @param address address to read
/// @return data there.

function RCARead(address as integer)
	data as integer
	data = _RCAMemory[address && _RCAMEMMASK]
endfunction data 

/// Write back to memory. Can be modified for read only memory and/or partly decoded memory.
/// @param address address to write
/// @param data data to write

function RCAWrite(address as integer,data as integer)
	_RCAMemory[address && _RCAMEMMASK] = data
endfunction 

// Fetch a single instruction byte.
// @return opcode fetched.

function _RCAFetch()
	opcode as integer
	opcode = _RCAMemory[_RCA_R[_RCA_P] && _RCAMEMMASK]							// Read the next opcode
	_RCA_R[_RCA_P] = (_RCA_R[_RCA_P]+1) && _RCAMASK16 							// Increment the program counter.
endfunction opcode	

/// Execute a given number of RCA1802 instructions. For speed reasons we are counting instructions not
/// cycles, but most instructions are 2 cycles/16 clocks anyway. My cheapie phone has a clock equivalent
/// of about 8-9Mhz.
/// @param instructions number of instructions to execute.

function RCAExecute(instructions as integer)
	opcode as integer
	r as integer
	n as integer
	while instructions > 0
		dec instructions 														// Decrement instruction counter.
		opcode = _RCAMemory[_RCA_R[_RCA_P] && _RCAMEMMASK]						// Read the next opcode
		_RCA_R[_RCA_P] = (_RCA_R[_RCA_P]+1) && _RCAMASK16 						// Increment the program counter.
		select opcode && 0xF0 													// Select on upper 4 bits of opcode.
			case 0x00															// 0x00 LDN (Load via)
				r = opcode && 0x0F								
				if r > 0 then _RCA_D = _RCAMemory[_RCA_R[r] && _RCAMEMMASK]
			endcase
			case 0x10															// 0x10 INC (Increment R)
				r = opcode && 0x0F								
				_RCA_R[r] = (_RCA_R[r] + 1) && _RCAMASK16
			endcase
			case 0x20															// 0x20 DEC (Decrement R)
				r = opcode && 0x0F								
				_RCA_R[r] = (_RCA_R[r] - 1) && _RCAMASK16								
			endcase
			case 0x30															// 0x30 Bxx (Various short branches)
				n = _RCAFetch() 												// Fetch the target address
				r = _RCAEvaluateCondition(r && 7) 								// Evaluate the test condition.
				if (opcode && 8) then n = (n = 0) 								// if 38-3F becomes inverse test.
				if n <> 0
					_RCA_R[_RCA_P] = (_RCA_R[_RCA_P] && _RCA_RCAMASKUPPER) || n // If passed the test then do a short branch.
				endif
			endcase
			case 0x40 															// 0x40 LDA (Load Advance)
				r = opcode && 0x0F								
				_RCA_D = _RCAMemory[_RCA_R[r] && _RCAMEMMASK]
				_RCA_R[r] = (_RCA_R[r] + 1) && _RCAMASK16
			endcase
			case 0x50															// 0x50 STR (Store)
				r = opcode && 0x0F								
				RCAWrite(_RCA_R[r],_RCA_D)
			endcase
			case 0x60 															// 0x60 I/O Mostly.
				if opcode = 0x60 												// 0x60 is IRX (actually OUT 0 does it as a side effect)
					_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] + 1) && _RCAMASK16
				endif
				if opcode >= 0x61 and opcode <= 0x67 							// 0x61-0x67 is OUT n (M(R(X))->Bus, Inc R(X))
					RCAAccessHardware(RCA_IOWRITE+opcode - 0x60,_RCAMemory[_RCA_R[_RCA_X]])
					_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] + 1) && _RCAMASK16
				endif
				if opcode >= 0x69 and opcode <= 0x6F 							// 0x69-0x6F is IN n (Bus -> M(R(X)),D)
					_RCA_D = RCAAccessHardware(RCA_IOREAD+opcode-0x68,0)
					RCAWrite(_RCA_R[_RCA_X],_RCA_D)
				endif
			endcase
			case 0x70															// 0x7x decoded individually.
				_RCAOpcode7x(opcode)
			endcase
			case 0x80															// 0x80 GLO (Get Lower)
				r = opcode && 0x0F								
				_RCA_D = _RCA_R[r] && _RCAMASK8
			endcase
			case 0x90															// 0x90 GHI (Get Higher)
				r = opcode && 0x0F								
				_RCA_D = (_RCA_R[r] >> 8) && _RCAMASK8
			endcase
			case 0xA0															// 0xA0 PLO (Put Lower)
				r = opcode && 0x0F								
				_RCA_R[r] = (_RCA_R[r] && _RCA_RCAMASKUPPER) || _RCA_D
			endcase
			case 0xB0															// 0xB0 PHI (Put Higher)
				r = opcode && 0x0F								
				_RCA_R[r] = (_RCA_R[r] && _RCAMASK8) || (_RCA_D << 8)
			endcase
			case 0xC0															// 0xCx LBR/LSK
				if opcode = 0xCC 												
					n = _RCA_IE <> 0 											// $CC code is if IE set (this is LSIE)
				else
					n = _RCAEvaluateCondition(opcode && 0x3) 					// Otherwise use bits 0..1 to determine test.
				endif
				if (opcode && 8) then n = (n = 0) 								// if C8-CF becomes inverse test.
				if (opcode && 4) = 0 											// C0-C3 is LBR
					r = _RCAFetch() << 8: r = r || _RCAFetch() 					// Fetch address.
					if n <> 0 then _RCA_R[_RCA_P] = r 							// And branch if the test was successful.
				else															// C4-C7 is LSK. However, the tests are backwards
					if n=0 then _RCA_R[_RCA_P] = (_RCA_R[_RCA_P]+2)&&_RCAMASK16 // e.g. C8 is LSKP unconditional. 
				endif
			endcase
			case 0xD0															// 0xD0 SEP (Set P)
				_RCA_P = opcode && 0xF
			endcase
			case 0xE0															// 0xE0 SEX (Set X)
				_RCA_X = opcode && 0xF
			endcase
			case 0xF0															// 0xFx decoded individually
				_RCAOpcodeFx(opcode)
			endcase
		endselect
	endwhile
endfunction	

//	Instructions in 70-7F are decoded individually.
//  @param opcode operation code

function _RCAOpcode7x(opcode as integer)
	n as integer
	select opcode
		case 0x70																// 0x70 RET
			n = _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
			_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] + 1) && _RCAMASK16
			_RCA_P = n && 0xF:_RCA_X = n >> 4:_RCA_IE = 1
		endcase
		case 0x71
			n = _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]						// 0x71 DIS
			_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] + 1) && _RCAMASK16
			_RCA_P = n && 0xF:_RCA_X = n >> 4:_RCA_IE = 0
		endcase
		case 0x72																// 0x72 LDXA
			_RCA_D = _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
			_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] + 1) && _RCAMASK16
		endcase
		case 0x73																// 0x73 STXD
			RCAWrite(_RCA_R[_RCA_X],_RCA_D)
			_RCA_R[_RCA_X] = (_RCA_R[_RCA_X] - 1) && _RCAMASK16
		endcase
		case 0x74 																// 74 ADC
			_RCA_D = _RCA_D + _RCA_DF + _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
		case 0x75																// 75 SDB
			_RCA_D = _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK] + (_RCA_D ~~ 0xFF) + _RCA_DF
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
		case 0x76																// 76 RSHR
			n = _RCA_D && 1
			_RCA_D = (_RCA_D >> 1) || (_RCA_DF << 7)
			_RCA_DF = n
		endcase
		case 0x77																// 77 SMB
			_RCA_D = _RCA_D + (_RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK] ~~ 0xFF) + _RCA_DF
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
		case 0x78 																// 78 SAV
			RCAWrite(_RCA_R[_RCA_X],_RCA_T)
		endcase
		case 0x79																// 79 MARK
			_RCA_T = (_RCA_X << 4) || _RCA_P
			RCAWrite(_RCA_R[2],_RCA_T)
			_RCA_X = _RCA_P
			_RCA_R[2] = (_RCA_R[2] - 1) && _RCAMASK16
		endcase
		case 0x7A																// 7A REQ
			_RCA_Q = 0
			RCAAccessHardware(RCA_IOWRITEQ,0) 									// Update the hardware line Q, set to zero.
		endcase
		case 0x7B																// 7B SEQ
			_RCA_Q = 1
			RCAAccessHardware(RCA_IOWRITEQ,1) 									// Update the hardware line Q, set to zero.
		endcase
		case 0x7C 																// 7C ADCI
			_RCA_D = _RCA_D + _RCA_DF + _RCAFetch()
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
		case 0x7D																// 7D SDBI
			_RCA_D = _RCAFetch() + (_RCA_D ~~ 0xFF) + _RCA_DF
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
		case 0x7E																// 7E RSHL
			_RCA_D = (_RCA_D << 1) || _RCA_DF
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && 0xFF
		endcase
		case 0x7F																// 7F SMBI
			_RCA_D = _RCA_D + (_RCAFetch() ~~ 0xFF) + _RCA_DF
			_RCA_DF = _RCA_D >> 8
			_RCA_D = _RCA_D && _RCAMASK8
		endcase
	endselect
endfunction

//	Instructions in F0-FF are decoded individually. F4-F7 and FC-FF co opt opcode 7x instructions
//	which are the same with DF preset or precleared.
//  @param opcode operation code

function _RCAOpcodeFx(opcode as integer)
	select opcode
		case 0xF0																// F0 LDX
			_RCA_D = _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
		endcase
		case 0xF1																// F1 OR
			_RCA_D = _RCA_D || _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
		endcase
		case 0xF2																// F2 AND
			_RCA_D = _RCA_D && _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
		endcase
		case 0xF3																// F3 XOR
			_RCA_D = _RCA_D ~~ _RCAMemory[_RCA_R[_RCA_X] && _RCAMEMMASK]
		endcase
		case 0xF4																// F4 ADD
			_RCA_DF = 0:_RCAOpcode7x(opcode)
		endcase
		case 0xF5																// F5 SD
			_RCA_DF = 1:_RCAOpcode7x(opcode)
		endcase
		case 0xF6																// F6 SHR
			_RCA_DF = 0:_RCAOpcode7x(opcode)
		endcase
		case 0xF7																// F7 SM
			_RCA_DF = 1:_RCAOpcode7x(opcode)
		endcase
		case 0xF8																// F8 LDI
			_RCA_D = _RCAFetch()
		endcase
		case 0xF9 																// F9 ORI
			_RCA_D = _RCA_D || _RCAFetch()
		endcase
		case 0xFA																// FA ANI
			_RCA_D = _RCA_D && _RCAFetch()
		endcase	
		case 0xFB																// FB XRI
			_RCA_D = _RCA_D ~~ _RCAFetch()
		endcase
		case 0xFC
			_RCA_DF = 0:_RCAOpcode7x(opcode)									// FC ADI
		endcase
		case 0xFD
			_RCA_DF = 1:_RCAOpcode7x(opcode)									// FD SDI
		endcase
		case 0xFE																// FE SHL
			_RCA_DF = 0:_RCAOpcode7x(opcode)
		endcase
		case 0xFF																// FF SMI
			_RCA_DF = 1:_RCAOpcode7x(opcode)
		endcase
	endselect
endfunction

// Evaluate an RCA1802 condition. Condition codes correspond to $30-$37 instructions
// @param conditionCode 	condition to test
// @return non zero if passes test.

function _RCAEvaluateCondition(conditionCode as integer)
	retVal as integer
	select conditionCode
		case 0 																	// 30 BR (always)
			retVal = 1
		endcase	
		case 1 																	// 31 BQ (Q flag set)
			retVal = (_RCA_Q <> 0)
		endcase	
		case 2 																	// 32 BZ (D zero)
			retVal = (_RCA_D = 0)
		endcase	
		case 3 																	// 33 BDF (DF set)
			retVal = (_RCA_DF <> 0)
		endcase	
		case default
			retVal = RCAAccessHardware(RCA_IOEFLAG + retVal-3, 0) <> 0			// 34-37 Bn (Query EFlag external)
		endcase
	endselect
endfunction retVal
