// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		binac_arithmetic.cpp
//		Purpose:	31 bit 2's Complement Binac Arithmetic functions
//					(also contains testing program buildable using #define TEST_PROGRAM)
//		Created:	15th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "binac_arithmetic.h"

#define TWOSCOMP(n)		((((n) ^ BITMASK) + 1) & BITMASK)				// 2's complement 31 bit
#define BITMASK 		(017777777777)									// 31 bit bit mask.
#define SIGN_BIT 		(1 << 30)										// Sign bit, 31 bit constant.

// *******************************************************************************************************************************
//													31 bit Binary Add
// *******************************************************************************************************************************

LONG32 BINMAdd(LONG32 n1,LONG32 n2) {	
	return (n1 + n2) & BITMASK;											// Simple 31 bit add
}

// *******************************************************************************************************************************
//												   31 Bit Binary Subtract
// *******************************************************************************************************************************

LONG32 BINMSubtract(LONG32 n1,LONG32 n2) {
	return (n1 + TWOSCOMP(n2)) & BITMASK;								// Simple 31 bit add using two's complement
}

// *******************************************************************************************************************************
//										31 Bit Binary Multiply, lower digits are thrown
// *******************************************************************************************************************************

LONG32 BINMMultiply(LONG32 n1,LONG32 n2) {
	int sign = 1;														// Sign of result
	LONG32 partialProduct = 0;											// Result

	if ((n1 & SIGN_BIT) != 0) {											// Remove sign from n1
		sign = -sign;n1 = TWOSCOMP(n1);
	}
	if ((n2 & SIGN_BIT) != 0) {											// Remove sign from n2
		sign = -sign;n2 = TWOSCOMP(n2);
	}

	for (int bit = 0;bit < 30;bit++) {									// For each bit
		if ((n1 & 1) != 0) {											// If set add n2 in
			partialProduct = (partialProduct + n2) & BITMASK;
		}
		n1 = n1 >> 1;													// Shift n1 right
		partialProduct = partialProduct >> 1;							// Shift product right
	}
	partialProduct = (sign<0) ? TWOSCOMP(partialProduct):partialProduct;// Put sign back
	return partialProduct | 1;											// Set rounding bit
}

// *******************************************************************************************************************************
//									31 Bit Binary Divide - divide by zero is not an error
// *******************************************************************************************************************************

LONG32 BINMDivide(LONG32 n1,LONG32 n2) {
	LONG32 result = 0;													// Result
	LONG32 p = SIGN_BIT;												// Adding bit
	for (int i = 0;i < 30;i++) {										// Work through bits
		if ((n1 & SIGN_BIT) != (n2 & SIGN_BIT)) {						// Comparing sign bits.
			n1 = (n1 + n1 + n2) & BITMASK;								// See Shaw's paper
		} else {
			n1 = (n1 + n1 - n2) & BITMASK;
			result |= p;
		}
		p = p >> 1;
	}
	return (result + 0x40000001) & BITMASK;								// Set fixup and rounding bit.
}

// *******************************************************************************************************************************
//									Convert 31 bit Binary to Floating Point equivalent
// *******************************************************************************************************************************

double BINMToFloat(LONG32 n) {
	if (n == 010000000000) return -1.0;									// Special edge case.
	double sign = 1.0;													// Sign of answer
	double total = 0.0;													// Total
	double addValue = 0.5;												// Bit value
	if ((n & SIGN_BIT) != 0) {											// If -ve, set sign and take 2's complement
		sign = -1;
		n = TWOSCOMP(n);		
	}
	for (int b = 0;b < 30;b++) {										// A bit at a time
		if ((n & (1 << (29-b))) != 0) {									// Add bit value if bit set
			total += addValue;
		}
		addValue = addValue / 2.0;										// Halve bit value
	}
	return total * sign;												// Put sign back.
}

// *******************************************************************************************************************************
//														TESTING CODE
// *******************************************************************************************************************************

#ifdef TEST_PROGRAM														// #define this to build executable

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TEST_COUNT 		1*1000*1000 									// Number of tests
#define DELTA 			2.0e-9											// Max error allowed for multiply/divide

#define SCALAR			pow(2.0,30)										// Scalar between FP and Integer
																		// Convert Binary to Float
#define TOBINARY(f) 	(f < 0) ? TWOSCOMP(int(fabs(f) * SCALAR)) : int(fabs(f) * SCALAR)
#define LONGRAND() 		((rand() << 16) ^ rand())						// 32 bit random value
																		// Assert check.
#define ASSERT(x) 		if (!(x)) exit(fprintf(stderr,"Assert: line %d (%d %d %G %G)",__LINE__,n1,n2,f1,f2))


int main() {
	srand(69);															// Seed random number generator.

	LONG32 n1,n2,r;
	double f1,f2;
	n1 = n2 = 0;f1 = f2 = 0.0;

	printf("Checking word size is 32 bit\n");
	ASSERT(sizeof(LONG32) == 4);										// Check it is the correct byte size (4)

	printf("Checking Float -> Binary conversion\n");
	ASSERT(TOBINARY(0.5) == 	   04000000000);						// Check basic float -> binary
	ASSERT(TOBINARY(pow(2,-15)) == 00000100000);
	ASSERT(TOBINARY(pow(2,-30)) == 00000000001);
	ASSERT(TOBINARY(-0.5) == 	    TWOSCOMP(04000000000));
	ASSERT(TOBINARY(-pow(2,-15)) == TWOSCOMP(00000100000));
	ASSERT(TOBINARY(-pow(2,-30)) == TWOSCOMP(00000000001));

	printf("Checking Binary -> Float conversion\n");					// Check binary -> float.
	for (int i = 0;i < TEST_COUNT;i++) {
		LONG32 v = LONGRAND() & BITMASK;
		double f = BINMToFloat(v);
		ASSERT(v == TOBINARY(f));
	}

	printf("Checking Addition\n");										// Binary Addition
	for (int i = 0;i < TEST_COUNT;i++) {
		do {
			n1 = LONGRAND() & BITMASK;n2 = LONGRAND() & BITMASK;
			f1 = BINMToFloat(n1);f2 = BINMToFloat(n2);
		} while (abs(f1 + f2) >= 1.0);
		r = BINMAdd(n1,n2);
		ASSERT(f1+f2 == BINMToFloat(r));
	}

	printf("Checking Subtraction\n");									// Binary Subtraction
	for (int i = 0;i < TEST_COUNT;i++) {
		do {
			n1 = LONGRAND() & BITMASK;n2 = LONGRAND() & BITMASK;
			f1 = BINMToFloat(n1);f2 = BINMToFloat(n2);
		} while (abs(f1 - f2) >= 1.0);
		r = BINMSubtract(n1,n2);
		ASSERT(f1-f2 == BINMToFloat(r));
	}

	printf("Checking Multiplication\n");								// Binary Multiplication
	for (int i = 0;i < TEST_COUNT;i++) {
		n1 = LONGRAND() & BITMASK;n2 = LONGRAND() & BITMASK;
		f1 = BINMToFloat(n1);f2 = BINMToFloat(n2);
		r = BINMMultiply(n1,n2);
		//printf("%G\n",fabs(f1*f2-BINMToFloat(r)));
		ASSERT(fabs(f1*f2-BINMToFloat(r)) < DELTA);
	}

	printf("Checking Division\n");										// Binary Division
	for (int i = 0;i < TEST_COUNT;i++) {
		do {
			n1 = LONGRAND() & BITMASK;n2 = LONGRAND() & BITMASK;
			f1 = BINMToFloat(n1);f2 = BINMToFloat(n2);
		} while (fabs(f1/f2) >= 1.0 || f2 == 0.0);
		r = BINMDivide(n1,n2);
		//printf("%G\n",fabs(f1/f2-BINMToFloat(r)));
		ASSERT(fabs(f1/f2-BINMToFloat(r)) < DELTA);
	}
	return 0;
}

#endif