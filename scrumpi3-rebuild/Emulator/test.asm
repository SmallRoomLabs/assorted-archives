
		cpu	sc/mp

buildPage = 0x70 									; page built into, upper byte

keyboardPort = 0x7C00 								; keyboard port
uartPort = 0x7D00 									; UART port
videoRAM = 0x7E00 									; video RAM
insRAM = 0x7F00 									; 8154 RAM.

;		xFC8-xFE7 		32 byte stack
;		xFE8-xFEF 		8 label addresses
;		xFF0-xFF1 		unused
;		xFF2 			current command
;		xFF3 			lines to display
;		xFF4,5 			address to display / etc.
;		xFF6 			Cursor position
;       xFF7-xFFF       Storage for Registers (Monitor only)

stack 	= 0x7FE8 									; stack ends here (use this because it pre-decrements)
labels 	= 0x7FE8 									; labels here (8 bytes)
command = 0x7FF2 									; current command.
listCount = 0x7FF3 									; lines to list.
workAddress = 0x7FF4 								; address working from (low byte, high byte)
cursor 	= 0x7FF6 									; cursor position.


registers = 0x0FF7 									; registers (these 2 are accessed via direct mode)
r_a 	= registers 								; hence the "wrong" values here.
r_e 	= registers+1								; the assembler has another bug.
r_s 	= registers+2
r_p1 	= registers+3
r_p2 	= registers+5
r_p3 	= registers+7

; ***************************************************************************************************************************************************
;
;															ROM Monitor Start
;
; ***************************************************************************************************************************************************

		org 	0x0000
		nop
		ldi 	buildPage 							; shift to page 7 (e.g. 70xx)
		xpah 	p0
		jmp		MonitorStart 						; go to the cold start

; ***************************************************************************************************************************************************
;
;														Run code and ReEnter Monitor.
;
;	There are two entry points. One to run from where you left off (continue) and one to run from a particular position in P3 (RunFromP3)
;	The position in P3 is -1 from the actual position because of SC/MPs pre-incrementing program counter.
;
; ***************************************************************************************************************************************************

Continue:
		ld 		r_p1+1 								; load P1
		xpah 	p1
		ld 		r_p1
		xpal 	p1
		ld 		r_p2+1 								; load P2
		xpah 	p2
		ld 		r_p2
		xpal 	p2
		ld 		r_p3+1 								; load P3
		xpah 	p3
		ld 		r_p3
		xpal 	p3
		ld 		r_e 								; load E
		xae
		ld 		r_s 								; load S
		cas
		ld 		r_a 								; load A

		xppc 	p3 									; run the program.

		st 		r_a 								; come back here to restart, save A,E,S
		csa
		st 		r_s
		lde
		st 		r_e
		xpah 	p1 									; save pointer register p1
		st 		r_p1+1
		xpal 	p1
		st 		r_p1
		xpah 	p2 									; save pointer register p2
		st 		r_p2+1
		xpal 	p2
		st 		r_p2
		xpal 	p3
		st 		r_p3+1
		xpah 	p3 									
		st 		r_p1
;
;		TODO:Status dump and list.
;
		jmp 	CommandLoop

; ***************************************************************************************************************************************************
;
;													(G) and	(C)ontinue commands
;
; ***************************************************************************************************************************************************

GoToAddress:
		lde 										; get the LSB
		st 		r_p3								; store into the P3 values that are loaded.
		xpah 	p1									; same for MSB.
		st 		r_p3+1								; so it is continue with a fixed address.

VisitContinue:
		jmp 	Continue

; ***************************************************************************************************************************************************
;
;															Monitor Start Routine
;
;	C 		Continue from where we left off.
;	G xxxx	Go from xxxx
;	I xxxx 	Implant at xxxx then, either, (i) 	type a 2 digit number the INT to load into memory
;										  (ii) 	press INT to go to the next.
;										  (iii) =n to store the address in label n
;									      (iv)  ?n display the offset to label n (no change)
;	H xxxx 	48 byte hex dump.
;	L 		Display labels.
;
; ***************************************************************************************************************************************************

MonitorStart:	
		ldi 	(Stack & 255)						; set the stack up in P2.
		xpal 	p2
		ldi 	(Stack / 256)
		xpah 	p2
		ldi 	(PrintStringInline-1) & 0xFF 		; print string in line.
		xpal 	p3
		ldi 	((PrintStringInline-1) / 256) | buildPage
		xpah 	p3
		xppc 	p3
		db 		1,"SCRUMPI 3",4

CommandLoop:
		ldi 	(PrintStringInline-1) & 0xFF 		; print command prompt.
		xpal 	p3
		ldi 	((PrintStringInline-1) / 256) | buildPage
		xpah 	p3
		xppc 	p3 						
		db 		13,"COMMAND ? ",4

		ldi 	labels & 255 						; set work address P1 to point to labels.
		xpal 	p1
		ldi 	labels/256
		xpah 	p1

		ldi 	(GetKbd-1) & 255
		xpal 	p3
		ldi 	((GetKbd-1) / 256 ) | buildPage
		xpah 	p3
		xppc 	p3									; read command key
		st 		command-stack(p2) 					; save it.
		xri 	'L'									; is it (L)abels
		jz 		List 								; list those 8 bytes out.
		xri 	'L'!'C'								; is it (C)ontinue
		jz 		VisitContinue 

		ldi 	((GetHexNibble-1) & 255)			; now read the hex nibble in, four times.
		xpal 	p3
		ldi 	((GetHexNibble-1) / 256) | buildPage
		xpah 	p3

		xppc 	p3 									; read first byte
		jp 		CommandLoop
		xppc 	p3
		jp 		CommandLoop
		lde 										; save in P1.H
		xpah 	p1						
		xppc 	p3 									; read second byte into E.
		jp 		CommandLoop
		xppc 	p3
		jp 		CommandLoop

		ld 		command-stack(p2) 					; read command
		xri 	'G'									; is it G(o)
		jz 		GoToAddress 				
		xri 	'G'!'I'								; is it I(mplant)
		jz 		Implant
		xri 	'I'|'H'								; is it H(ex dump)		
		jnz 	CommandLoop

		lde
		xpal 	p1 									; make P1 point to the listing.
		ldi 	7 									; we want to do 8 of them.
;
;		List i+1 lines from P1
;
List:	ccl 										; add 1 to the count.
		adi 	1
		jmp 	List

Implant:jmp 	Implant

		org 	0x600

; ***************************************************************************************************************************************************
;
;										VDU Out routine. Preserves everything except CY/L and OV
;
;	Understands 01 (Clear) 08 (Backspace) 0D (CR/LF) and characters 20..FF (characters). All others are ignored.
;
;	Really tight because it uses relative addressing to access 'cursor' variable. If needs be, memory can be grabbed by moving the reloading of p2/p3
; 	above to later, when p1 can be used as a pointer.
; ***************************************************************************************************************************************************

PutVDU:	st 		@-1(p2) 							; save A on stack
		ld 		cursor 								
		xpal 	p1									; save P1.L on stack, set P1.L to cursor
		st 		@-1(p2)
		ldi 	buildPage | (videoRAM/256) 			; save P1.H on stack,make P1 point to Video Memory/cursor
		xpah 	p1
		st 		@-1(p2)
_PVPutVDUEcho: 										; come here from keyboard routine to echo as stack already set up and P1.H is pointing to video RAM
;
;		Now test for control characters
;
		ld 		2(p2) 								; reload A (character to be printed)
		xri 	0x01								; was it $01 (Clear screen)
		jz 		_PVClear
		xri 	0x01 ! 0x08 						; was it $08 (Backspace)
		jz 		_PVBackspace
		xri  	0x08 ! 0x0D 						; was it $0D (Carriage Return)
		jz 		_PVCarriageReturn
;
;		We know now it isn't a known control character, check for any unknown ones :) All those XRIs
; 		haven't changed the upper 3 bits which is what we want to test now.
;
		ani 	0xE0 								; if all 000x xxxx e.g. it is a control character
		jz 		_PVExit 							; exit the routine.
;
;		Output the character and advance the cursor
;
_PVCharacter:
		ld 		2(p2) 								; reload character to be written
		ani 	0xBF 								; clear bit 6 - (may be necessary if we change graphics for unused space in VDU RAM)
		st 		@1(p1)								; write to screen, incrementing cursor which is in P1.L
		xpal 	p1 									; retrieve cursor value from P1.L
		jz 		_PVScroll							; if zero wrapped round go to bottom line and scroll up, note P1.H will now be $7F not $7E.
_PVSaveCursorAndExit:
		st 		cursor 								; write cursor position back and exit
		jmp 	_PVExit
;
;		Carriage return to the start of the next line. Scroll if you reach the bottom.
;
_PVCarriageReturn:
		xpal 	p1 									; get cursor position out of P1.L
		ani 	0xE0 								; start of current line, clear lower 5 bits.
		ccl 										; move to next line, 32 characters down.
		adi 	0x20 
		jnz 	_PVSaveCursorAndExit 				; not at top now, so save and exit
		jmp 	_PVScroll 							; if at top we've wrapped around to top so need to scroll.
;
;		Backspace to previous screen position, if possible of course.
;
_PVBackSpace:
		xpal 	p1 									; get cursor out of P1.L
		jz 		_PVExit 							; can't backspace before position zero
		dld 	cursor 								; cursor back one and exit.
		jmp 	_PVExit
;
;		Clear Screen. On entry from start, A is zero because of XRI checks. P1.H points to video memory.
; 		so actually move cursor to A and clears screen from A on.This is also used by the scroll
; 		up routine to clear the bottom line and set the cursor at the start of the bottom line.
;
_PVClear:
		st 		cursor 								; save cursor position as where we are clearing from.
_PVClearLoop:
		xpal 	p1 									; update P1 									
		ldi 	' ' 								; write space and increment it
		st 		@1(p1) 								
		xpal 	p1 									; until P1.L is zero again, e.g. at the bottom of the screen.
		jnz 	_PVClearLoop 						; then fall through to the exit.
;
;		Exit the PUTVDU Routine
;
_PVExit:
		ld 		@1(p2) 								; restore P1.H off stack
		xpah 	p1
		ld 		@1(p2)								; restore P1.L off stack
		xpal 	p1
		ld 		@1(p2) 								; restore A
		xppc 	p3 									; return 
		jmp 	PutVDU 								; is reentrant.
;
;		Scroll screen up, then use clear screen to clear bottom line and position cursor.
;
_PVScroll:
		ldi 	buildPage | (videoRAM/256) 			; set P1.H to 7C again - if cursor overflowed, will be P1.H will be $7F here
		xpah 	p1
		ldi 	0xE0 								; and scroll up, because of the XRI test, this is effectively zero
_PVScrollLoop:
		xri 	0xE0 								; undo XRI test, copy to P1.L
		xpal 	p1 
		ld 		32(p1) 								; copy from next line
		st 		@1(p1) 								; to this line, and go to next cell
		xpal 	p1 									; retrieve P1.L
		xri 	0xE0 								; check if bottom line.
		jnz 	_PVScrollLoop 						; keep going til 0xE0 reached.
		ldi 	0xE0 								; start fill from bottom line.
		jmp 	_PVClear 							; set cursor and clear screen from 0xE0

; ***************************************************************************************************************************************************
;	
;							Keyboard Read Routine. Echos keypress on display. Preserves P1,P2,E returns key in A.
;
;											This is *not* re-entrant as it exits through PutVDU
;
;		-1(p2) 	Counter for flashing cursor
; 		-2(p2) 	Last key state. Non zero if key pressed, 0 if no key pressed.
;		2(p2) 	Output key, removed from stack when PutVDU exits.
;
; ***************************************************************************************************************************************************


GetKbd:	st 		@-1(p2) 							; save A - we overwrite this with the key stroke.
		xpal 	p1									; save P1.L and P1.H on stack 
		st 		@-1(p2) 							; these are restored when we exit echo.
		xpah 	p1 
		st 		@-1(p2)
		ldi 	1 									; set last key state to 1, must have no key before key acknowledged.
		st 		-2(p2) 								
		scl 										
;
;		Keyboard loop. At this point, if carry is clear clear cursor and echo using print routine
;		If carry is set, continue with keyboard scanning
;
_GKLoop:ldi 	(cursor & 255)						; set P1 to point to cursor
		xpal 	p1
		ldi 	(cursor / 256) | buildPage 			; in page 7
		xpah 	p1
		ld 		0(p1) 								; read the cursor
		xpal 	p1									; put in P1.L 
		ldi 	videoRAM/256 						; make P1 point to the cursor in video memory
		xpah 	p1
		ldi 	' '									; write a blank space there
		st 		0(p1)
		csa 										; check carry flag
		jp 		_PVPutVDUEcho 						; if clear, then echo and exit.
		ild 	-1(p2) 								; increment the counter.
		jp 		_GKNoToggle
		ldi 	' ' + 0x80 							; set solid block on
		st 		0(p1)
_GKNoToggle:
		ldi 	keyboardPort/256 					; point P1 to keyboard port
		xpah 	p1
		ldi 	0 									; set keyboard scan value to $00
		st 		2(p2)
		ldi 	8 									; first address row to check. 
_GKLoadRow:
		xpal 	p1
		ld 		0(p1)								; read the row.
		ani 	0xF 								; only interested in lower 4 bits.
		jnz 	_GKFoundKey 						; if a bit is set, then go figure out what the key is.
		ld 		2(p2)								; add 4 to keyboard scan value, e.g. next row down.
		ccl 
		adi 	4
		st 		2(p2)
		xpal 	p1 									; get bit to check 
		sr 											; shift it right.
		jnz 	_GKLoadRow 							; not completed, do the next row.

		ldi 	13 									; going to check code $0D (carriage return) attached to SA.
		st 		2(p2)
		csa 										; look at Sense A, which is connected to the INT key on the keyboard.
		ani 	0x10
		jnz 	_GKCheckState

		st 		-2(p2) 								; set last key state to zero.
_GKPauseAndLoopBack:
		scl  										; we haven't finished yet.
		dly 	255 								; pause
		jmp 	_GKLoop
;
;		Key pressed. 2(P2) contains the row (0,4,8,12), A the bits read.
;
_GKFoundKey:
		sr 											; shift right
		jz 		_GKGotKeyNumber 					; if zero, must've shifted last '1' bit that time.
		xpal 	p1 									; save in P1.L
		ild 	2(p2) 								; increment the keyboard number
		xpal 	p1 									; restore from P1.L
		jmp 	_GKFoundKey							; go round again
;
;		Now 2(P2) contains the raw key number 0-15.
;
_GKGotKeyNumber:
		ld 		0(p1) 								; read the shift bits 4,5,6
		ani 	0x70 								; we only want those.
		xri 	0x40 								; 0x,1x,2x,4x -> 4x,5x,6x,0x
		jnz 	_GKNotPunc							; 4x,5x,6x,0x -> 4x,5x,6x,3x
		ldi 	0x30
_GKNotPunc:
		ccl
		adi 	0xF0 								; 3x,4x,5x,2x now - numbers, alpha1, alpha2, punc
		or 		2(p2) 								; or with current key
		st 		2(p2) 								; 2 now contains the current key as read.
_GKCheckState:
		ld 		-2(p2) 								; look at last key state 
		jnz 	_GKPauseAndLoopBack 				; if non-zero, we are still waiting gor a release, so pause & loop back.

		ccl 										; we want out :)
		jmp 	_GKLoop 							; so loop back with carry clear.

; ***************************************************************************************************************************************************
;
;						Print A as Hexadecimal. 2 versions of this, one with and one without trailing space.
;
;	Note: always reentrant to PrintHex.
; ***************************************************************************************************************************************************

PrintHexTrailingSpace:
		st 		@-1(p2)								; push A on stack
		ldi 	' ' 								; print a space after it.
		jmp 	_PHEntry
PrintHex:
		st 		@-1(p2) 							; push A on stack
		ldi 	0x1F 								; control character
_PHEntry:
		st 		@-1(p2) 							; save A on stack
		xpah 	p3 									; save P3 on stack.
		st 		@-1(p2)
		xpal 	p3
		st 		@-1(p2)
		ldi 	(PutVDU-1) & 255 					; set P3 to call Put VDU
		xpal	p3
		ldi 	(PutVDU-1) / 256
		xpah 	p3
		ld 		3(p2) 								; get number,do MSB
		sr
		sr
		sr
		sr
		ccl
		dai 	0x90 	
		dai 	0x40
		xppc 	p3
		ld 		3(p2)								; get number, do LSB
		ani 	0x0F
		ccl
		dai 	0x90
		dai 	0x40
		xppc 	p3		
		ld 		2(p2) 								; print trailer
		xppc 	p3 									; it will ignore the control character $1F and print the space.
_PHNoSpace:
		ld 		@1(p2)								; restore P3
		xpal 	p3
		ld 		@2(p2)								; 2 here, dumping the space byte.
		xpah 	p3
		ld 		@1(p2) 								; restore A
		xppc 	p3
		jmp 	PrintHex 						

; ***************************************************************************************************************************************************
;
;		Shift A single hex digit into E. (To get two, just call xppc p3 twice !). Returns A +ve error, A -ve good, bits 0-6 key pressed
;
; ***************************************************************************************************************************************************

GetHexNibble:
		ldi 	((GetKbd-1) / 256) | buildPage		; save P3 on stack and set it to keyboard read
		xpah 	p3 									
		st 		@-1(p2)
		ldi 	(GetKbd-1) & 255 					; set P3 to get keyboard, call it to read a character.
		xpal 	p3
		st 		@-1(p2)

		xppc 	p3 									; read the key.
		st 		-1(p2) 								; save it.
		ccl 
		adi 	255-'F' 							; if key >= F, will now be +ve
		jp 		_GHKError
		adi 	6 									; now will be 1,2,3,4,5 for A-F
		jp 		_GHKIsAlphabeticHex
		adi 	7 									; if +ve now will be wrong.
		jp 		_GHKError
_GHKIsAlphabeticHex:
		ccl 	
		adi 	10 									; now should be 0-15 if >= '0'
		jp 		_GHKIsValidDigit
_GHKError:
		ccl 										; return clear carry to return 
		jmp 	_GHKExit 

_GHKIsValidDigit:
		xae 										; E = new digit, A = old
		rr 											; rotate right x 4
		rr
		rr
		rr
		ani 	0xF0 								; mask off upper bits.
		ore 										; or the new value in.
		xae 										; put back in E
		scl 										; set carry/link as good.		
_GHKExit:
		ld 		@1(p2)								; restore P3
		xpal 	p3
		ld 		@1(p2)								
		xpah 	p3
		csa 										; copy carry/link bit into A indicating error.
		ani 	0x80 								; clear bits 0-6
		or 		-3(p2) 								; or old character in.
		xppc 	p3									; return
		jmp 	GetHexNibble						; and it is re-entrant.

; ***************************************************************************************************************************************************
;
;												Print following string inline in code. Breaks P1.
;
; ***************************************************************************************************************************************************

PrintStringInline:
		ldi 	(PutVdu-1) & 255 					; put putvdu.l into P3, save old P3 in P1
		xpal 	p3
		xpal 	p1
		ldi 	((PutVdu-1) / 256) | buildPage 		; do the same with the upper byte
		xpah 	p3
		xpah 	p1
		ld 		@1(p1) 								; P1 will point to the XPPC P3 that called this.
_PSILLoop:
		ld 		@1(p1) 								; read next character
		xppc 	p3 									; print it
		xri 	0x04 								; end marker ?
		jnz 	_PSILLoop
		ld 		@-1(p1)								; point P1 back to the 04 marker, which is followed by code
		xppc 	p1 									; return to that caller
		jmp 	PrintStringInline

