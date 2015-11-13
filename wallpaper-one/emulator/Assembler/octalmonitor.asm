; ******************************************************************************************************************
;
;											Octal Monitor with Display.
;
; ******************************************************************************************************************

AddressHigh = 0 														; 0x800 Address 
AddressLow = 1 															; 0x801 Address Low
Cursor = 2 																; 0x802 Vertical Cursor position
Counter = 3 															; 0x803 Counter of horizontal screen position.
DigitCount = 4 															; 0x804 Digits to print
RowCount = 5 															; 0x805 Rows to print
DigitsHigh = 6 															; 0x806 Digits (High)
DigitsLow = 7 															; 0x807 Digits (Low)
LineCounter = 8 														; 0x808 Line Counter 
LinesToDraw = 9 														; 0x809 Number of lines to draw

		cpu	sc/mp

		org 	0x0000
		nop
		csa 															; short beep.
		ori 	0x04
		cas
		dly 	0xFF
		ani 	0xF8
		cas

; ******************************************************************************************************************
;												Clear the screen
; ******************************************************************************************************************

		ldi 	0x00 													; set P1 to screen top.
		xpal 	p1
		ldi 	0x0C ! 0x10 						
ClearScreen:															; fill 0C00 to 0FFF
		xpah 	p1
		ldi 	0
		st 		@1(p1)
		xpah 	p1
		xri 	0x10
		jnz 	ClearScreen

		ldi 	0x08													; Set P1 to point to 0x800
		xpah 	p1

; ******************************************************************************************************************
;										Set up to edit from 0x8010 onwards
; ******************************************************************************************************************

		ldi 	0x10 													; set start address to 0x810
		st 		AddressLow(p1)
		ldi 	0x00
		st 		AddressHigh(p1)
		ldi 	0x00													; reset cursor
		st 		Cursor(p1) 		
		jmp 	RedrawScreen

CharacterSet:
		include ..\fonts\octal_mon_font.asm

; ******************************************************************************************************************
;											Redraw the whole display
; ******************************************************************************************************************

RedrawScreen:
		ldi 	0														; clear line counter
		st 		LineCounter(p1)
		ldi 	0x0C 													; point P2 to 0xC3C - 2nd line (6 lines of 5)
		xpah 	p2														; last character
		ldi 	0x5C+1
		xpal 	p2
		ldi 	5														; reset the number to draw.
		st 		LinesToDraw(p1)

; ******************************************************************************************************************
;												Do next line
; ******************************************************************************************************************

NextLine:
		ldi 	1 														; printing two digits.
		st 		Counter(p1)

; ******************************************************************************************************************
;												Do next number
; ******************************************************************************************************************

DrawLoop:
		ld 		AddressLow(p1) 											; put address+counter in digits and P3
		ccl 
		add 	LineCounter(p1)
		st 		DigitsLow(p1)
		xpal 	p3
		ld 		AddressHigh(p1)
		adi 	0
		st 		DigitsHigh(p1)
		xpah 	p3
		ld 		Counter(p1)												; get counter 0: Address 1:Data
		jz 		DrawDigits
		ld 		0(p3) 													; read data at address
		st 		DigitsLow(p1)
		ldi  	0														; MSB is zero.
		st 		DigitsHigh(p1)
DrawDigits:
		ld 		Counter(p1) 											; 0 address 1 data
		xri 	3 														; 3 address, 2 data
		st 		DigitCount(p1) 											; number of times we print it.

; ******************************************************************************************************************
;													Do next digit.
; ******************************************************************************************************************

NextDigit:		
		ldi 	5														; rows to print
		st 		RowCount(p1)

		ld 		DigitsLow(p1) 											; read digits low into A
		ccl
		add 	DigitsLow(p1) 											; double it
		ani		15 														; lower 3 bits
		ccl
		adi 	CharacterSet-LoadPoint-1								; offset from character - loadpoint
		xae 															; put in E.
LoadPoint:
		db 		0xC0,0x80 	; LD E(P0)									; read the low byte, put in P3.L
		xpal 	p3
		xae 
		scl 	
		cai 	7
		xae
		db 		0xC0,0x80 	; LD E(P0)									; read the high byte, put in P3.H
		xpah 	p3

; ******************************************************************************************************************
;													Output bit data
; ******************************************************************************************************************

outbit 	macro 	after
		xpah 	p3
		rrl
		xpah 	p3
		xpal 	p3
		st 		@after(p2)
		rrl 	
		xpal 	p3
		endm

DrawRow:
		outbit	1														; output 3 bits, at end go to next row.
		outbit 	1
		outbit 	30
		dld 	rowCount(p1) 											; do it 5 times
		jnz 	DrawRow
		ld 		@-100(p2) 												; back 32 x 5 + 4 = 164
		ld 		@-64(p2)												; previous character on start row.

		ldi 	3 														; shift the number right 3 times
		st 		rowCount(p1)
ShiftRight:
		ld 		DigitsHigh(p1)
		rrl
		st 		DigitsHigh(p1)
		ld 		DigitsLow(p1)
		rrl
		st 		DigitsLow(p1)
		rrl
		dld 	rowCount(p1)
		jnz 	ShiftRight


		dld		DigitCount(p1)
		jp 		NextDigit 												; do the next digit.

		ld 		@-2(p2) 												; space between digits.
		dld 	Counter(p1) 											; decrement counter
		jp 		DrawLoop 												; do the next item.

		ld 		@110(p2) 												; point to next line
		ld 		@114(p2)
		ild 	LineCounter(p1) 										; next line
		dld 	LinesToDraw(p1)
		jz 		RepaintCursor
		ldi 	(NextLine-1) & 255 										; jump to Nextline, same page
		xpal 	p0

; ******************************************************************************************************************
;													Redraw the marker
; ******************************************************************************************************************

RepaintCursor:
		ldi 	0x00
		xpal 	p2
		ldi 	0xC!0x10 												; repoint P2 to video
_ClearCursor:
		xpah 	p2
		ldi 	0x00 													; now clear the 2 pixels on the RHS.
		st 		@1(p2)
		st 		@31(p2)
		xpah 	p2
		xri 	0x10
		jnz 	_ClearCursor

		xpal 	p2
		ldi 	0x0C 													; P2 back to video start
		xpah 	p2

; ******************************************************************************************************************
;												Draw the actual marker
; ******************************************************************************************************************
		
		ld 		Cursor(p1) 												; read cursor
_FindCursorPos
		jz 		_FoundCursorPos
		xae 															; save in E
		ld 		@96(p2)													; down 6 lines.
		ld 		@96(p2)
		xae 															; get back.
		scl 															; subtract one
		cai 	1 														
		jmp 	_FindCursorPos
_FoundCursorPos:
		ld 		@96(p2)													; now redraw the cursor.
		ldi 	1	
		st 		0(p2)
		st 		32(p2)
		st 		33(p2)
		st 		64(p2)

; ******************************************************************************************************************
;												Handle player input
; ******************************************************************************************************************

WaitRelease:															; wait for SA, SB and Keypad to be released.
		csa
		ani 	0x30
		jnz 	WaitRelease
		xpal 	p2
		ldi 	0x0C 													; wait for toggle release
		xpah 	p2
		ld 		0(p2)													; read them
		jnz 	WaitRelease

WaitKey:
		csa 															; check if A pressed
		ani 	0x10
		jnz 	APressed
		jmp 	WaitKey 												; still waiting .....


; ******************************************************************************************************************
;										Handle pressing of 'A' (advance)
; ******************************************************************************************************************

APressed:
		ild 	Cursor(p1)												; increment cursor
		xri 	5 														; reached 6
		jnz 	RepaintCursor 											; no, repaint it.
		st 		Cursor(p1) 												; zero cursor
		ccl
		ld 		AddressLow(p1)											; add four to address
		adi 	4
		st 		AddressLow(p1)
		ld 		AddressHigh(p1)
		adi 	0
		st 		AddressHigh(p1)
		ldi 	(RedrawScreen-1) & 255 									; long jump back to redraw the whole screen.
		xpal 	p2
		ldi 	(RedrawScreen-1) / 256
		xpah  	p2
		xppc 	p2
Wait:	jmp 	Wait

