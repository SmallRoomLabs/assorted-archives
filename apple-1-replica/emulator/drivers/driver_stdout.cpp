// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		driver_stdout.c
//		Purpose:	Output Driver, StdOut only. Really a debugging thing.
//		Created:	20th October 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#include <stdio.h>
#include "driver_common.h"

// *******************************************************************************************************************************
//												Ultra simple STDOUT Driver
// *******************************************************************************************************************************

void DRVWrite(BYTE8 command,DRVPARAM data) {
	switch(command) {
		case DWA1_WRITE:	printf("%c",data);break;
		case DWA1_BACKSPACE:printf("[<-]");break;
		case DWA1_NEWLINE:	printf("[CR]\n");break;
	}
}

