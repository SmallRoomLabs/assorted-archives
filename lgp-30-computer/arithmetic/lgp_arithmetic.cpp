//*******************************************************************************************************
//*******************************************************************************************************
//
//      Name:       lgp_arithmetic.cpp
//      Purpose:    Mathematical functions for LGP-30
//      Author:     Paul Robson
//      Date:       18th October 2015
//
//*******************************************************************************************************
//*******************************************************************************************************

#include "lgp_arithmetic.h"

#define MATH_RESULT_MASK  (0xFFFFFFFEL) 										// The LSB is always zero.

static LONG32 nMultiplyResult; 													// N-Multiply stored here.

//*******************************************************************************************************
//										Negate a number
//*******************************************************************************************************

LONG32 MATHNegate(LONG32 number) {
	return ((number ^ 0xFFFFFFFFL) + 1) & MATH_RESULT_MASK;						// 2's complement.
}

//*******************************************************************************************************
//							Add two numbers, may return overflow
//*******************************************************************************************************

LONG32 MATHAdd(LONG32 n1,LONG32 n2) {
	//LGPMATH_ASSERT((n2 & 1) == 0);												// LSB must be zero.
	LONG32 result = (n1 + n2) & MATH_RESULT_MASK;								// Calculate the results
	BYTE8 sn1, sn2, sr; 														// Get the signs
	sn1 = n1 >> 31;sn2 = n2 >> 31;sr = result >> 31;
	if (sn1 == sn2 && sn1 != sr) result = MATH_ERROR;							// Overflow occurred.
	return result;
}

//*******************************************************************************************************
//			  Multiply two 32 bit numbers, calculate M and N multiply
//*******************************************************************************************************

LONG32 MATHMultiply(LONG32 n1,LONG32 n2) {
	//LGPMATH_ASSERT((n2 & 1) == 0);												// LSB must be zero.
	BYTE8 sn1,sn2;
	sn1 = MATHIsNegative(n1);sn2 = MATHIsNegative(n2); 							// Get the signs of the numbers
	if (sn1 != 0) n1 = MATHNegate(n1);											// Make the two values positive
	if (sn2 != 0) n2 = MATHNegate(n2);

	LONG64 result = n1; 														// Calculate the result
	result = result * n2;

	n1 = (LONG32)result;														// get result (64 bits)
	n1 = n1 & MATH_RESULT_MASK;													// truncate as normal
	nMultiplyResult = n1;														// and store the n-multiply result.

	n1 = ((LONG32)(result >> 31)) & MATH_RESULT_MASK;							// Get the result back as a 32 bit value.
	if (sn1 != sn2) n1 = MATHNegate(n1);										// Put the sign back.
	return n1;
}

//*******************************************************************************************************
//									  Get N-Multiply result
//*******************************************************************************************************

LONG32 MATHGetNMultiply(void) {
	return nMultiplyResult;
}

//*******************************************************************************************************
//									Divide two 32 bit longs
//*******************************************************************************************************


LONG32 MATHDivide(LONG32 n1,LONG32 n2) {
	if (n2 == 0) return MATH_ERROR;												// Divide by zero is an overflow.
	//LGPMATH_ASSERT((n2 & 1) == 0);												// LSB must be zero.
	BYTE8 sn1,sn2;
	sn1 = MATHIsNegative(n1);sn2 = MATHIsNegative(n2); 							// Get the signs of the numbers
	if (sn1 != 0) n1 = MATHNegate(n1);											// Make the two values positive
	if (sn2 != 0) n2 = MATHNegate(n2);
	if (((double)n1) / ((double)n2) >= 1.0) return MATH_ERROR;					// Overflow.
	LONG64 result = n1; 														// Calculate the result
	result = (result << 31) / n2;
	// TODO: Rounding to the nearest bit in thirtieth place, see pp27
	n1 = ((LONG32)result) & MATH_RESULT_MASK;									// Make it a 32 bit long
	if (sn1 != sn2) n1 = MATHNegate(n1);										// Put the sign back.
	return n1;
}