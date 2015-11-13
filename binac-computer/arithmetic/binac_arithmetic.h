// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		binac_arithmetic.h
//		Purpose:	31 bit 2's Complement Binac Arithmetic functions (header)
//		Created:	15th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _BINAC_ARITHMETIC_H
#define _BINAC_ARITHMETIC_H

typedef unsigned int LONG32;
LONG32 BINMAdd(LONG32 n1,LONG32 n2);
LONG32 BINMSubtract(LONG32 n1,LONG32 n2);
LONG32 BINMMultiply(LONG32 n1,LONG32 n2);
LONG32 BINMDivide(LONG32 n1,LONG32 n2);
double BINMToFloat(LONG32 n);

#endif