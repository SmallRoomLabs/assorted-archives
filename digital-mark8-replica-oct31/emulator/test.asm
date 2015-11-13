	
		cpu		8008new

TVPort = 0eh
CassetteOutputPort = 08h
CassetteInputPort = 01h
KeyboardPort = 00h


		org 	0

		mvi 	l,message & 255
		mvi		h,message / 256
		call 	BIOSPrintString

		mvi 	h,buffer / 256
		mvi 	l,buffer & 255
		call 	BIOSReadString
		hlt 	

message:
		db 		12,"Hello world",13,13,0

		db 		255
buffer:	ds 		4
		db 		255

		align 	256

; ******************************************************************************************************************************
;
;	Read string into HL. Handles Backspaces. On entry HL points to the first character of a buffer ; the previous character and
;	the character at the end of the buffer must both have bit 7 set, this delimits the type-in. 
;
;	so db 255,0,0,0,0,255 specifies a three-character buffer (plus space for zero), and HL should point to the first zero. 
;	The buffer must all be in one page. Only characters 32-127 can be in the buffer. 
;
;	BCHL unchanged.
;
; ******************************************************************************************************************************

BIOSReadString:
		mvi 	a,31	 										; output the cursor (2 wavy lines)
		call 	BIOSCharacterOut 
_BIRSWaitKey:
		in 		KeyboardPort 									; wait for keyboard press
		ora 	a
		jp 		_BIRSWaitKey 
		ani 	07Fh 											; drop bit 7
		mov 	m,a 											; write it out at current character position.
_BIRSWaitRelease:
		in 		KeyboardPort 									; wait for it to be released
		ora 	a
		jm 		_BIRSWaitRelease
		mvi 	a,8 											; delete the cursor character
		call 	BIOSCharacterOut

		mov 	a,m 											; retrieve typed character.
		cpi 	8 												; was the character chr(8)
		jz 		_BIRSBackspace 									; if so, backspace.
		mvi 	m,0 											; replace it with a terminating zero.
		cpi 	13 												; was the character chr(13), e.g. a new line
		jz 		_BIRSExit										; if so, exit.
		mov 	m,a 											; put character back

		inr 	l 												; bump L to next space
		mov 	a,m 											; read it
		dcr 	l 												; fix L back up
		ora 	a 												; if the value is >= 0x7F then we cannot accept it, no room for \0
		jm 		BIOSReadString 									; so, try again.

		mov 	a,m 											; read the character typed.

		cpi 	32 												; if < 32 then ignore it, go back
		jc 		BIOSReadString

		mov 	m,a 											; write the character back.
		inr 	l 												; advance to next space
_BIRSPrintAndLoop
		call 	BIOSCharacterOut 								; print it.
		jmp		BIOSReadString 									; and get the next character.

_BIRSBackspace:
		dcr 	l 												; go back one.
		mov 	a,m 											; read character there
		inr 	l 												; fix L back up
		ora 	a 												; is that character >= 0x7F e.g. we've reached the beginning
		jm 		BIOSReadString 									; if so, we ignore it.
		dcr 	l 												; back one character
		mvi 	a,8 											; print chr(8) and loop back
		jmp 	_BIRSPrintAndLoop
;
_BIRSExit: 														; keep working backwards till we find the beginning marker.
		dcr 	l
		mov 	a,m
		ora 	a
		jp 		_BIRSExit
		inr 	l 												; point to first character
		ret

; ******************************************************************************************************************************
;
;	Print the string at HL. Returns A = 0, HL points to the terminating zero, and preserves BC
;
; ******************************************************************************************************************************

BIOSPrintString:
		mov 	a,m
		ora 	a
		rz 
		call 	BIOSCharacterOut
		inr 	l
		jnz 	BIOSPrintString
		inr 	h
		jmp 	BIOSPrintString

; ******************************************************************************************************************************
;
;	Print any of the 128 characters out, in A, at the current cursor position. A scrolling terminal built out of Dr Sudings
;	32 x 8 TV Typewriter.
;
;								A = 12 		Clear screen
;								A = 13 		Carriage Return/Line Feed
; 								A = 8 		Backspace (not if at top left of screen already.)
;
; 								To print characters 12,13 and 8 as characters or them with 080h.
;
;	Preserves A,B,C,H and L.
;
; ******************************************************************************************************************************

BIOSCharacterOut:
		mov 	e,l 											; copy HL to DE
		mov 	d,h
		mvi 	l,BICO_SaveALH & 255 							; point HL to save area
		mvi 	h,BICO_SaveALH / 256 
		mov 	m,a 											; save A,L,H there
		inr 	l
		mov 	m,e 
		inr 	l
		mov 	m,d
		mvi 	l,BICO_Cursor & 255 							; HL now points to cursor position

		cpi 	8 												; is it 8 (backspace) 
		jz 		_BICOBackspace

		mvi 	e,0 											; E is the position-loopback-mask (see below)
		cpi 	12 												; is the character 12 ? (clear screen)
		jnz 	_BICONotClearScreen 
;
;		Set up for clear screen. Mask is %11111111, e.g. do till bottom of screen, and we also home the cursor here.
;
		mov 	m,e 											; reset cursor position to $00
		dcr 	e 												; E is $FF, e.g. write spaces for the while screen
		mov 	a,e 											; output that $FF, which homes the cursor
		out 	TVPort
		xra 	a 												; output $00 which is the required transition.										
		out 	TVPort	
		mvi 	a,' '											; print spaces till at 00 again (done by setting E to $FF)
_BICONotClearScreen:
;
;		Set up for carriage return. Mask is %00011111 e.g. do till end of line.
;
		cpi 	13 												; is the character 13 ?
		jnz 	_BICOLoop
		mvi 	a,' '											; print space rather than code 13.
		mvi 	e,01Fh 											; and fill till this mask is zero e.g. EOL.
;
;	Writes a single character in A out. E contains the 'loop mask' ; if after the character print, the cursor position anded
;	with this mask is non-zero, print spaces until it is zero - this is used to clear the screen (E = %11111111) and to 
;	do a carriage return (E = %00011111)
;
_BICOLoop:
		mvi 	l,BICO_Cursor & 255 							; point HL to the cursor
		mvi 	h,BICO_Cursor / 256 							
		ori 	080h 											; set bit 7 so it is written.
		mov 	d,a 											; save it in D
		out 	TVPort 											; write to TV Display
		xra 	a 												; and send the trailing zero.
		out 	TVPort
		mov 	a,d 											; restore character written to A.
		mov 	d,m 											; read cursor position into D.
		inr 	d 												; advance it and write it back.
		mov 	m,d
		dcr 	d 												; D is now the original position we wrote to.
		mov 	l,d 											; L is now the original position
		mvi 	h,BICO_Mirror/256 								; HL now points to the position in the mirror we should write to.
		mov 	m,a 											; save character written in the mirror.

		inr 	l 												; point back to the new cursor position
		mov 	a,l
		ana 	e 												; do we go round again ?
		mvi 	a,' '											; if so, next time we print a space (doesn't change Z flag)
		jnz 	_BICOLoop 		

		inr 	e 												; if E was $FF then don't scroll when at the bottom (as we've just cleared...)
		jz 		_BICOExit
		mov 	a,l 											; if L, the new cursor position is zero, then scroll up rather than wrap round.
		ora 	a 												; which is what happens if you just go straight to _BICOExit here - you go to the
		jnz 	_BICOExit 										; top line and carry on.
;
; 		At this point, we scroll the whole screen up. Firstly we scroll the mirror contents page and clear the bottom line.
;
		mov 	e,a 											; make E is destination, D source
		mvi 	d,32 											; hence E = 0, D = 32
_BICOCopy:
		mov 	l,d 											; HL points to source
		mov 	a,m 											; read it
		mov 	l,e 											; HL points to destination
		mov 	m,a 											; write it back.
		mov 	a,e 											; look at destination
		cpi 	224 											; if >= 224 e.g. bottom line
		jc 		_BICONotClear
		mvi 	m,' '!080h 										; fill it with spaces.
_BICONotClear:
		inr 	d 												; bump source and destination
		inr 	e 												; until source is back at zero. 												
		jnz 	_BICOCopy	 									; when source zero, have done the whole copy up.
;
;		Then we repaint the whole screen from the top. E is already zero, whcih means the whole screen.
;
		call 	_BICORepaintAll 								; after this the whole screen has been repainted, E is zero already.
		mvi 	e,224 											; then we do it again 224 times to put the write position on the bottom line
		call 	_BICORepaintAll 								; we can only position the cursor by repainting the whole screen.

_BICOExit:
		mvi 	l,BICO_SaveALH & 255 							; point HL to save area
		mvi 	h,BICO_SaveALH / 256 
_BICORestore:
		mov 	a,m 											; restore AHL back.
		inr 	l
		mov 	e,m
		inr 	l
		mov 	h,m
		mov 	l,e
		ret 													; and exit.
;
;		Backspace
;
_BICOBackspace:
		mov 	a,m 											; read cursor position
		ora 	a 												; is zero, e.g. top of screen ?
		jz 		_BICOExit 										; if so, we cannot backspace.
		mov 	e,a 											; put the old cursor position in E and D
		mov 	d,e
		dcr 	d 												; D is now the new cursor position
		mov 	l,d 											; Make HL point to the previous character
		mvi 	h,BICO_Mirror/256 								
		mvi 	m,' '!080h 										; write a space there to erase it.
		call 	_BICORepaintAll 								; repaint everything including the current cursor position, erases.
		mov 	e,d 											; then do it again with one fewer, putting cursor in correct position.
		call 	_BICORepaintAll
		jmp 	_BICOExit 										; and exit.

;
;		Repaint E characters on the screen (note E=0 = all characters) and set cursor position to E.
;
_BICORepaintAll:
		mvi 	l,BICO_Cursor & 255 							; point HL to the cursor
		mvi 	h,BICO_Cursor / 256 							
		mov 	m,e 											; update cursor position.
		mvi 	a,0FFh 											; send the home cursor 
		out 	TVPort
		xra 	a
		out 	TVPort
		mov 	l,a 											; point HL to the mirror
		mvi 	h,BICO_Mirror/256
_BICORepaintLoop:
		mov 	a,m 											; read character and print it
		out 	TVPort
		xra 	a
		out 	TVPort 
		inr 	l 												; advance pointer
		dcr 	e 												; do this E times.
		jnz 	_BICORepaintLoop
		ret

;
;		Data Segment
;
		align 	256 											; screen mirror, has to be on 1/4k boundary. required for scrolling/backspace.
BICO_Mirror:
		ds 		256

BICO_SaveALH:
		db 		0,0,0 											; save position for AHL
BICO_Cursor:
		db 		0 												; current cursor position.
