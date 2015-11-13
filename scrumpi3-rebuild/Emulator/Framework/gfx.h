// *******************************************************************************************************************************
// *******************************************************************************************************************************
//
//		Name:		gfx.h
//		Purpose:	Support library for SDL (Header)
//		Created:	1st September 2015
//		Author:		Paul Robson (paul@robsons.org.uk)
//
// *******************************************************************************************************************************
// *******************************************************************************************************************************

#ifndef _GFX_H
#define _GFX_H

#include <SDL.h>

#define GFXKEY_BASE 		(1)

#define GFXKEY_UP			(GFXKEY_BASE+1)
#define GFXKEY_DOWN			(GFXKEY_BASE+2)
#define GFXKEY_LEFT			(GFXKEY_BASE+3)
#define GFXKEY_RIGHT		(GFXKEY_BASE+4)
#define GFXKEY_LSHIFT		(GFXKEY_BASE+5)
#define GFXKEY_CONTROL		(GFXKEY_BASE+6)
#define GFXKEY_F1			(GFXKEY_BASE+7)
#define GFXKEY_F2			(GFXKEY_BASE+8)
#define GFXKEY_F3			(GFXKEY_BASE+9)
#define GFXKEY_F4			(GFXKEY_BASE+10)
#define GFXKEY_F5			(GFXKEY_BASE+11)
#define GFXKEY_F6			(GFXKEY_BASE+12)
#define GFXKEY_F7			(GFXKEY_BASE+13)
#define GFXKEY_F8			(GFXKEY_BASE+14)
#define GFXKEY_F9			(GFXKEY_BASE+15)
#define GFXKEY_F10			(GFXKEY_BASE+16)
#define GFXKEY_RETURN		(GFXKEY_BASE+17)
#define GFXKEY_BACKSPACE	(GFXKEY_BASE+18)
#define GFXKEY_TAB			(GFXKEY_BASE+19)
#define GFXKEY_RSHIFT		(GFXKEY_BASE+20)
#define GFXKEY_SHIFT		(GFXKEY_BASE+21)

#define GFXISMODIFIERKEY(x)	(GFXISCONTROLKEY(x) || GFXISSHIFTKEY(x))
#define GFXISSHIFTKEY(x)	((x) == GFXKEY_SHIFT || (x) == GFXKEY_RSHIFT || (x) == GFXKEY_LSHIFT)
#define GFXISCONTROLKEY(x)	((x) == GFXKEY_CONTROL)

#define GRID(x,y) 			_GFXX(x),_GFXY(y)
#define GRIDSIZE 			_GFXS()

int _GFXX(int x);
int _GFXY(int y);
int _GFXS(void);

void GFXOpenWindow(char *title,int width,int height,int colour);
void GFXStart(void);
void GFXCloseWindow(void);

void GFXRectangle(SDL_Rect *rc,int colour);
void GFXCharacter(int xc,int yc,int character,int size,int colour,int back);
void GFXString(int xc,int yc,char *text,int size,int colour,int back);
void GFXNumber(int xc,int yc,int number,int base,int width,int size,int colour,int back);
int  GFXIsKeyPressed(int character);
int  GFXToASCII(int ch);
void GFXSetCharacterSize(int xSize,int ySize);
void GFXDefineCharacter(int nChar,int b1,int b2,int b3,int b4,int b5);
void GFXXRender(SDL_Surface *surface);

#endif
