	
		cpu		8008new



COScopeDisplay = 10o

; ******************************************************************************************************************************
;													OS ROM
; ******************************************************************************************************************************

		jmp 	_BootMonitor 	
COCharacterOut:	
		jmp 	_COCharacterOut
COStringOut:
		jmp 	_COStringOut

; ******************************************************************************************************************************
;
;									3 x 5 pixel font table, on a 128 byte boundary
;
; ******************************************************************************************************************************

COFontTable:																; this needs to be in the same page.
		include fonttable.asm

; ******************************************************************************************************************************
;
;		Output character in A. Preserves A,B,C,H and L. D and E lost.
;
;		0..31 		Control characters (13 = CR, 12 = Clear, 8 = Backspace, others ignored)
;		32..95 		Normal characters
;		96..127		Mapped onto 64-95 hence lower -> upper
; 		128..191 	Set horizontal position
;		192..255 	Set vertical position
;
; ******************************************************************************************************************************

_COCharacterOut:
		mov 	d,h 														; save old HL in DE
		mov 	e,l
		mvi 	h,_COHLSave / 256 											; point HL to restore point
		mvi 	l,_COHLSave & 255
		mov 	m,e 														; save HL there - note the page issue here
		inr 	l 															; potentially.
		mov 	m,d
		inr 	l 															; HL now points to the X position.
		mov 	e,a 														; save character in E

		mvi 	a,201o 														; turn scanning off.
		out 	COScopeDisplay
;
;		set the x,y position on the display.
;
COHPositionM1:
		mov 	a,m 														; read current horizontal (2nd byte modified)
		out 	COScopeDisplay 												; which is STX
		inr 	l 															; point to Y position.
COVPositionM1:
		mov 	a,m 														; read current vertical (2nd byte modified)
		dcr 	l 															; back pointing at X position.
		adi 	5 															; five down as we draw character upwards
		ori 	0100o 														; make set Y command
		out 	COScopeDisplay 												; set Y
;
;		Check and process character
;	
		mov 	a,e 														; get character back.
		ora 	a 															; check sign.
		jm 		COSetPosition 												; 0x80 ... 0xFF are set H/V.

		cpi 	060h 														; if L/C, make U/C
		jc 		CONotLower
		sui 	32 															; difference between a and A
CONotLower:
		sui 	32 															; 32-95 becomes 0-63
		jm 		COControl 													; if is no negative it was 0-31.
		add 	a 															; double it
		adi		COFontTable & 255 											; make HL point to the font table entry
		mov 	l,a 														; save in L
		mvi 	h,COFontTable / 256
		mov 	a,m 														; read char description low into A
		inr 	l 	
		mov 	h,m 														; read char description high into H
		mov 	l,a 														; HL now contains character description
;
;		Macro outputs the LSB of HL rotating it right. Can use draw/clear with or without the advance.
;
outbit 	macro 	cmdbase
		mov 	a,h 														; rotate HL right into carry.
		rar
		mov 	h,a
		mov 	a,l
		rar 
		mov 	l,a

		mvi 	a,0	 														; A = 1 = draw, 0 = clear
		ral
		xri 	cmdbase 													; construct command
		out 	COScopeDisplay 												; display it.
		endm
;
;		Rendering loop - 3 bits at a time working upwards
;
CONextRow:
		outbit	205o
		outbit 	205o
		outbit 	203o

		mvi 	a,200o 														; dec Y
		out 	COScopeDisplay
		mvi 	a,207o 														; dec X twice
		out 	COScopeDisplay
		out 	COScopeDisplay

		mov 	a,h 														; when we only have 01 left, then we are
		ora 	l 															; finished rendering.
		cpi 	1
		jnz 	CONextRow

		mvi 	h,XCursor / 256 											; point HL to X position
		mvi 	l,XCursor & 255
		mov 	a,m 														; get X position back
		adi 	4 															; advance by 4
		mov 	m,a
		cpi 	61 															; if < 61 then position is okay, exit.
		jc 		COExit		
;
;		Macro required for filling.
;
out4 	macro 																; output same character four times.
		out 	COScopeDisplay
		out 	COScopeDisplay
		out 	COScopeDisplay
		out 	COScopeDisplay
		endm
;
;		Execute a CR, on entry requires HL to point to the H-X position.
;
COCarriageReturn: 															; do CR, on entry HL should point to H Position
		xra 	a 															; zero Acc
		out 	COScopeDisplay 												; set X Position to 0.
		mov 	m,a															; do a carriage return, set X to zero.
		inr 	l 															; point HL to Y position
		mov 	a,m 														; add 6 to that
		adi 	6
		cpi 	57 															; if < 59 a new line will fit.
		jc 		CONoHome
		xra 	a 															; back to top of screen
CONoHome:
		mov 	m,a 														; update Y position
		ori 	0100o 														; make it a STY command
		out 	COScopeDisplay
		mvi 	d,24 														; fill this x 16 
;
;		Draw 16 x D blanks from the current position. Used for clearing new line on CR and clearing screen.
;
COFill:																		; print d x 16 blanks, if 24 this is one line.
		mvi 	a,205o														; command is clear pixel and advance X.
COFillLoop:
		out4 																; in total, output 16 times.	
		out4
		out4
		out4
		dcr 	d
		jnz 	COFillLoop
;
;		Exit point. Done this way so align above
;
COExit:	
		mvi 	a,206o 														; screen back on.
		out 	COScopeDisplay
		mov 	a,e 														; restore A
		mvi 	h,_COHLSave / 256 											; point HL to restore point
		mvi 	l,_COHLSave & 255
CORestoreHLFromHLViaE:
		mov 	e,m 														; reload HL using E
		inr 	l
		mov		h,m
		mov 	l,e 	
		ret
;
;		Handle control characters. On entrance, A has had 32 subtracted from it hence comparing 13-32
;		HL points to the X position.
;
COControl:
		cpi 	13-32 														; was it a CR ?
		jz 		COCarriageReturn 											; go to the carriage return code.
		cpi 	8-32 														; was it backspace ?
		jz 		COBackSpace 												; go to the backspace code.
		sui 	12-32 														; was it clear screen ?
		jnz 	COExit														; exit if not.
		mov 	m,a 														; clear X, as the result of sui will be zero if CLR
		inr 	l 															; point HL to Y position
		mov 	m,a 														; clear that as well
		mov 	d,a 														; we want 256 x 16 blanks = 4096
		jmp 	COFill 														; doesn't matter where we start
;
;		Backspace, moves back one character and falls through to exit.
;
COBackSpace:
		mov 	a,m 														; get X position
		sui 	4															; back 4
		jp 		CONotLeftEdge  												; if -ve now
		xra 	a 															; set X as 0
CONotLeftEdge:
		mov 	m,a 														; update X position and fall through
		jmp 	COExit
;
;		Character codes 128..255 set X and Y position.
;
COSetPosition:
		jmp 	COExit
		add 	a 															; put bit 6 into bit 7
		jp 		COSPSkip
		inr 	l 															; now point HL to Y position.
COSPSkip:
		mov 	a,e 														; get character back
		ani 	03Fh 														; only interested in lower 6 bits
		mov 	m,a 														; write to X or Y accordingly.
		jmp 	COExit

; ******************************************************************************************************************************
;
;				Print String at HL. Preserves B,C. On exit, A = 0, HL points to character after string end.
;
; ******************************************************************************************************************************

_COStringOut:										
		mov 	a,m 														; read character
		inr 	l 															; bump HL
		jnz 	_CSNoCarry
		inr 	h
_CSNoCarry:
		ora 	a 															; return if zero
		rz
		call 	COCharacterOut 												; print character
		jmp 	COStringOut

_BootMonitor:
		mvi 	a,2 														; disable keyboard interrupt
		in 		0 										

		mvi 	h,BootText/256
		mvi 	l,BootText&255
		call 	COStringOut
		hlt

BootText:	
		db 		12,"ECS-8008",13,13,0


; ******************************************************************************************************************************
;													RAM Data
; ******************************************************************************************************************************

; ------------------------------------------------------------------------------------------------------------------------------
_COHLSave:																	; storage for HL save, CharacterOut.
		dw 		0
XCursor:																	; horizontal cursor position
		db 		0
YCursor:																	; vertical cursor position.
		db 		0
; ------------------------------------------------------------------------------------------------------------------------------


