// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		flexowriter.cpp
//		Purpose:	Flexowriter Emulation
//		Created:	10th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include "sys_processor.h"
#include "iomanager.h"
#include "flexowriter.h"

#include <stdio.h>

BYTE8 isUpperMode = 0;

BYTE8 FLXRead(BYTE8 *buffer,BYTE8 size) {
	buffer[0] = 'A';
	buffer[1] = 'B';
	buffer[2] = 'C';
	buffer[3] = 'D';
	buffer[4] = 'E';
	return 5;
}

static const BYTE8 __codeToCharacter[] = {
	#include "mapping/codeToChar.h"
};

void FLXWrite(BYTE8 ch) {
	printf("Writing %oO %dD to flexowriter.\n",ch,ch);
	if (ch == 047) isUpperMode = 1;
	if (ch == 057) isUpperMode = 0;
	// 045 CR
	// 061 BS
	// 061 TAB
	BYTE8 index = ch * 2;
	if (isUpperMode == 0) index++;
	ch = __codeToCharacter[index];
	if (ch == 0) return;
	printf("ASCII %c %dD\n",ch,ch);
}
