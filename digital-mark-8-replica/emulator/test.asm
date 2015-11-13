	
		cpu		8008new
		org 	0

lxi 	macro 	address
		mvi 	l,address & 255
		mvi 	h,address / 256
		endm

vduPort   = 14
		lxi 	message
loop:	mov 	a,m
		ora 	a
		jz 		Wait
		call 	PrintCharacter
		inr 	l
		jmp 	loop

Wait:	jmp 	Wait

message:;
		;db		12,"Hello world !",13,"Line 2",13,13,13,13,13,13,"]>>",13,"Scrolled Line",8,8,"Hello",0
		db 		12,"Mark-8 OS v1.0",13,0	
		
PrintCharacter:
		mov 	d,h 												; copy HL to DE
		mov 	e,l
		lxi 	_pcTempHL 											; point HL to save point
		mov 	m,e 												; save old HL in temporary
		inr 	l
		mov 	m,d

		mvi 	l,cursor & 255 										; point HL to the cursor.

		cpi 	8 													; 8 (Backspace)
		jz 		_PCBackspace
		cpi 	12 													; 12 (Clear Screen)
		jz 		_PCClearScreen	
		cpi 	13													; 13 (Carriage Return)
		jz 		_PCReturn 
		cpi 	32 													; ignore other control characters
		jc 		_PCExit

		mov 	e,a 												; save character in E
		ori 	080h 												; output it to the VDU
		out 	vduPort
		xra 	a
		out 	vduPort
		mov 	d,m 												; read cursor position
		inr 	d 													; bump and write back
		mov 	m,d

		mov 	l,d 												; HL now points to the cursor shadow memory
		dcr 	l 													; unpick the write.
		mvi 	h,shadowVDU / 256
		mov 	m,e 												; write the character out.
		mov 	a,d 												; get the new cursor value back
		ora 	a 													; if it is zero we need to scroll.
		jz 		_PCScrollScreen 
_PCExit:															; exit print.
		lxi 	_pcTempHL
LoadHLFromHL:														; Load HL from (HL) using E.
		mov 	e,m
		inr 	l
		mov 	h,m
		mov 	l,e
		ret
;
;		Clear Screen
;
_PCClearScreen:
		lxi		shadowVDU 											; point HL to shadow VDU
_PCCSFill:
		mvi 	m,' '												; fill shadow memory with spaces.
		inr 	l
		jnz 	_PCCSFill
		call	_PCRepaintWholeScreen 								; repaint the screen 
		call 	_PCHomeCursor 										; home cursor
		jmp 	_PCExit 											; and exit.
;
;		Home Cursor
;
_PCHomeCursor:
		mvi 	a,0FFh 												; physically home the cursor
		out 	vduPort
		xra 	a
		out		vduPort 	
_PCSaveAToCursor:
		lxi 	cursor 												; point HL to cursor
		mov 	m,a 												; write it.
		ret
;
;		Repaint whole screen
;
_PCRepaintWholeScreen:
		xra 	a
;
;		Repaint the screen as until the physical cursor = A (e.g. A = 0 = whole screen)
;
_PCRepaintScreenToA:
		mov 	e,a 												; save target in E.
		call	_PCHomeCursor 										; home cursor first.
		mov 	a,e
		call 	_PCSaveAToCursor 									; set where the cursor is going.
		lxi 	shadowVDU 											; point HL to shadow VDU
_PCRSTALoop:
		mov 	a,m 												; copy shadow memory to display
		ori 	80h
		out 	vduPort
		xra 	a
		out 	vduPort
		inr 	l 													; next cell
		dcr 	e 	 												; done all of them.
		jnz 	_PCRSTALoop
		ret
;
;		Carriage return.
;
_PCReturn:
		mov 	l,m 												; L contains cursor position
		mvi 	h,shadowVDU / 256 									; HL points to shadow.
_PCEndOfLine:
		mvi 	a,' '+80h 											; print a space.
		out 	vduPort
		xra 	a
		out 	vduPort
		inr 	l 													; next column
		mov 	a,l
		ani 	1Fh 												; reached end of line
		jnz 	_PCEndOfLine
		mov 	a,l 												; get position
		call 	_PCSaveAToCursor 									; save in cursor position
		ora 	a 
		jnz 	_PCExit 											; exit if not at bottom of screen.
;
;		Scroll whole screen up one line.
;
_PCScrollScreen:
		mvi 	e,020h 												; point H to shadow RAM, E is pointer.
		mvi 	h,shadowVDU / 256 									
_PCCopyUp:
		mov 	l,e 												; HL points to byte to copy
		mov 	d,m 												; read into D.
		mov 	a,e 												; work out where it goes, one line up
		sui 	20h
		mov 	l,a 												
		mov 	m,d
		inr 	e
		jnz 	_PCCopyUp 											; do the whole screen.
		mvi 	l,0E0h  											; erase the bottom line
_PCClearBottom:
		mvi 	m,' '
		inr 	l
		jnz 	_PCClearBottom
		call	_PCRepaintWholeScreen 								; repaint the whole screen.
		mvi 	a,0E0h 												; and do it again, put cursor on bottom line.
		call 	_PCRepaintScreenToA
		jmp 	_PCExit
;
;		Backspace
;
_PCBackSpace:
		mov 	a,m 												; get cursor position
		ora 	a 													; if zero, top of screen, so can't backspace.
		jz 		_PCExit
		sui 	1
		call 	_PCRepaintScreenToA 								; reposition cursor
		jmp 	_PCExit


		align 	256
cursor: 
		db 		0 													; physical position of cursor.
_pcTempHL:
		dw 		0 													; HL saved during print character.

		align 	256
shadowVDU: 															; shadows the memory inside the TVT so we can scroll etc.
		ds 		256

