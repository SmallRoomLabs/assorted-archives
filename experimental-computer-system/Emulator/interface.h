// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		interface.c
//		Purpose:	Scelbi controls/display interface
//		Created:	10th September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

BYTE8 IFRead(BYTE8 item);
void IFWrite(BYTE8 item,BYTE8 data);
void IFReset(void);

#define IFR_KEYBOARD		(0x00)
#define IFR_INTERRUPTQUERY	(0x01)
#define IFW_LEFTDISPLAY		(0x10)
#define IFW_RIGHTDISPLAY	(0x11)
#define IFW_HOGENSON		(0x12)

#ifdef WINDOWS
#define WRITEDISPLAY(x,y,o) 	DBGXWriteDisplay(x,y,o)
void DBGXWriteDisplay(int x,int y,int isOn);
#endif