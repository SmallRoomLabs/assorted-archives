//*******************************************************************************************************
//*******************************************************************************************************
//
//      Name:       arithmetic_test.c
//      Purpose:    Test for the LGP-30 Arithmetic code. Most of these come from the programming
//					manual. I am assuming the LIBC routines work okay.
//      Author:     Paul Robson
//      Date:       18th October 2015
//
//*******************************************************************************************************
//*******************************************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LGPMATH_ASSERT(test) if (!(test)) exit(fprintf(stderr,"Assert failed at %d.\n",__LINE__))

#include "lgp_arithmetic.h"

//*******************************************************************************************************
//							Convert a shifted value to a 32 bit Long
//*******************************************************************************************************

LONG32 makeNumber(double n,BYTE8 q) {
 	LGPMATH_ASSERT(fabs(n) <= 0x3FFFFFFF);										// Check the n value is in range.
 	LONG32 result = (LONG32)(fabs(n)*pow(2,31-q)); 								// Work out result. (q = 32 special case)
 	return (n >= 0) ? result : MATHNegate(result);								// Return number, or -number.
}

//*******************************************************************************************************
//						Testing, examples from the LGP-30 Programming Manual
//*******************************************************************************************************

int main(int argc,char *argv[]) {
																				// Check the type sizes.
	printf("Checking type sizes correct.\n");
	LGPMATH_ASSERT(sizeof(BYTE8) == 1);
	LGPMATH_ASSERT(sizeof(WORD16) == 2);
	LGPMATH_ASSERT(sizeof(LONG32) == 4);
	LGPMATH_ASSERT(sizeof(LONG64) == 8);
																				// Check the makeNumber from n,q works correctly 
	printf("Checking (n,q) -> LONG32 format conversion works.\n");
	LGPMATH_ASSERT(makeNumber(19,5) == 0x4C000000L);							// p25.
	LGPMATH_ASSERT(makeNumber(2,2) ==  0x40000000L);							// p25-26
	LGPMATH_ASSERT(makeNumber(3,2) ==  0x60000000L);							
	LGPMATH_ASSERT(makeNumber(6,4) ==  0x30000000L);							// p26
	LGPMATH_ASSERT(makeNumber(3,30) == 0x00000006L);
	LGPMATH_ASSERT(makeNumber(4,32) == 0x00000002L);
	LGPMATH_ASSERT(makeNumber(-6,4) == 0xD0000000L);							// p27

	printf("Checking addition\n");												// Check addition.
	LGPMATH_ASSERT(MATHAdd(makeNumber(2,2),makeNumber(3,2)==makeNumber(5,2)));	// p26
	LGPMATH_ASSERT(MATHAdd(makeNumber(6,4),makeNumber(-6,4)==makeNumber(0,0)));	// p27 (implied)
	LGPMATH_ASSERT(MATHAdd(makeNumber(2,3),makeNumber(3,4)==0x38000000L));		// p50

	LONG32 maxInt = 0x7FFFFFFE; 												// Largest possible integer.
	LGPMATH_ASSERT(MATHAdd(maxInt,2) == MATH_ERROR);							// Check overflow.
	LGPMATH_ASSERT(MATHAdd(MATHNegate(maxInt),MATHNegate(4)) == MATH_ERROR);	// Check underflow.	


	printf("Checking subtraction\n");											// Check subtraction.
	LGPMATH_ASSERT(MATHSubtract(makeNumber(5,3),makeNumber(2,3)) 				// p50
													== makeNumber(3,3));
	LGPMATH_ASSERT(MATHSubtract(makeNumber(2,3),makeNumber(5,3)) 				// p50 extension to check complement.
													== makeNumber(-3,3));

	printf("Checking M-Multiply\n");											// Check M-Multiply

	LGPMATH_ASSERT(MATHMultiply(makeNumber(2,2),								// p51-2
										makeNumber(3,2)) == makeNumber(6,4));
	LGPMATH_ASSERT(MATHMultiply(makeNumber(3,3),								// p51-2
										makeNumber(2,4)) == makeNumber(6,7));
	LGPMATH_ASSERT(MATHMultiply(makeNumber(3.25,15),							// p52
										makeNumber(2,15)) == makeNumber(6,30));
	LGPMATH_ASSERT(MATHMultiply(makeNumber(3.00,15),							// p52 (truncation)
										makeNumber(2,15)) == makeNumber(6,30));

	LGPMATH_ASSERT(MATHMultiply(makeNumber(-3,3),								// extensions to check signs work okay
										makeNumber(2,4)) == makeNumber(-6,7));
	LGPMATH_ASSERT(MATHMultiply(makeNumber(-3.25,15),							
										makeNumber(2,15)) == makeNumber(-6,30));
	LGPMATH_ASSERT(MATHMultiply(makeNumber(-3.25,15),							
										makeNumber(-2,15)) == makeNumber(6,30));

	printf("Checking N-Multiply\n");											// Check N-Multiply
	LONG32 result;
	MATHMultiply(makeNumber(3,31),makeNumber(2,30));							// p 53-54
	LGPMATH_ASSERT(MATHGetNMultiply() == makeNumber(3,29));
	MATHMultiply(makeNumber(3.25,15),makeNumber(2,15));
	LGPMATH_ASSERT(MATHGetNMultiply() == 0x80000000);							// p54

	printf("Checking Divide\n");												// Check Divide.
	LGPMATH_ASSERT(MATHDivide(makeNumber(3,3),									// p54-5
								makeNumber(2,2)) == makeNumber(1.5,1));
	LGPMATH_ASSERT(MATHDivide(makeNumber(3,3),									// Check Division by Zero.
								makeNumber(0,1)) == MATH_ERROR);
	LGPMATH_ASSERT(MATHDivide(makeNumber(-3,3),
								makeNumber(2,2)) == makeNumber(-1.5,1));
	LGPMATH_ASSERT(MATHDivide(makeNumber(3,2),									// p54 Check overflow
								makeNumber(2,2)) == MATH_ERROR);

	printf("\n*** Arithmetic test completed. ***\n");										
	return (0);
}
