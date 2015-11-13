//*******************************************************************************************************
//*******************************************************************************************************
//
//      Name:       lgp_arithmetic.h
//      Purpose:    Mathematical functions for LGP-30 (Header)
//      Author:     Paul Robson
//      Date:       18th October 2015
//
//*******************************************************************************************************
//*******************************************************************************************************

#ifndef _LGP_ARITHMETIC_H
#define _LGP_ARITHMETIC_H

#ifndef LGP_BYTE_TYPES
#define LGP_BYTE_TYPES
typedef unsigned char BYTE8;													// Basic types required.
typedef unsigned short WORD16;
typedef unsigned int  LONG32;
typedef unsigned long long LONG64;
#endif

#define MATH_ERROR 		  (0x77777777L) 										// Arithmetic cannot return bit 0 set
																				// use odd numbers as error codes.
#define MATHSubtract(n1,n2) MATHAdd(n1,MATHNegate(n2))							// Subtraction defined as macro.
#define MATHIsNegative(n) 	(((n) >> 31) != 0)									// Check sign bit defined as macro.

LONG32 MATHNegate(LONG32 number);												// Negate
LONG32 MATHAdd(LONG32 n1,LONG32 n2);											// Add 
LONG32 MATHMultiply(LONG32 n1,LONG32 n2);										// Multiply (M and N calculated)
LONG32 MATHDivide(LONG32 n1,LONG32 n2);											// Divide
LONG32 MATHGetNMultiply(void);													// Get N Multiply if required.
#endif


