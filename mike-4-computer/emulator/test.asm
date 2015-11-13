	
		cpu		8008new

RAMSpace = 02000h 											; RAM starts here.

characterBuffer = RAMSpace+0 								; 32 byte buffer for display and input.
labelTable = RAMSpace + 32 									; 16 labels, 2 bytes each.
resetVectors = RAMSpace + 48 								; 8 reset vectors, 7 bytes each.

workSpace = RAMSpace + 69
tempHL_RS = workSpace	 									; temporary store for HL (RefreshScope screen)
tempHL_PC = workSpace+2 									; temporary store for HL (Print character)
cursor = workSpace+4 										; position of cursor.
currentAddress = workSpace+5 								; current active address
newAddress = workSpace+7 									; new entered address.
commandWord = workSpace+9 									; 5 bit packed command word (has to come after newAddress)


		org 	0h
		mov 	a,a 										; disables the interrupt, as it executes LAA.
		call 	PatchVectors 								; patch the vectors in.
		jmp 	MainLoop 									; go to start
Unused:	ret 												; unassigned vectors go here.

vector 	macro 	rstNumber,defaultValue
		org 	rstNumber * 8 								; this is where the reset is.
		jmp 	resetVectors+(rstNumber-1)*3 				; this is the code it executes on RST 8 etc.
		dw 		defaultValue 								; default value immediately follows.
		endm

PrintCharacter = 8 											; RST 08 : Print Character
		vector 	1,_PrintCharacter 							; 8 reset vectors.

		vector 	2,Unused
		vector 	3,Unused
		vector 	4,Unused
		vector 	5,Unused
		vector 	6,Unused
		vector 	7,Unused


; *******************************************************************************************************************
;				
;												Main Command Loop
;
; *******************************************************************************************************************
MainLoop:
		mvi 	a,12 										; clear screen
		rst 	PrintCharacter
		mvi 	a,9 										; tab across twice
		rst 	PrintCharacter
		mvi 	a,9
		rst 	PrintCharacter
		call	PrintCurrentAddress 						; print the current address
		call 	ReadStringNoClear							; read input line without clearing line.

Wait: 	jmp 	Wait

; *******************************************************************************************************************
;
;			Parse command in character buffer into 15 bit 5-bit command and a new address, possibly.
;	
; *******************************************************************************************************************

ParseInputCommand:
		mvi 	h,newAddress / 256
		mvi 	l,newAddress & 255 							; set new address to 0xFFFF (e.g. none entered)
		mvi 	m,0FFh
		inr 	l
		mvi 	m,0FFh
		inr 	l 											; zero the command word.
		xra 	a
		mov 	m,a
		inr 	l
		mov 	m,a
		mov 	b,a  										; B is an index into the command line.
_PICCommand:
		; check for A-Z 0-9 if so shift into command else skip.

_PICAddress:
		; check for 0-9 A-F if so shift into newaddress else skip, zeroing if -ve.
		; special case of '='
		ret

; *******************************************************************************************************************
;
;							Print current address as hex number, breaks ABCDE
;
; *******************************************************************************************************************

PrintCurrentAddress:
		mvi 	h,(currentAddress+1)/256 					; point to current address high
		mvi 	l,(currentAddress+1) & 255
		mov 	a,m 										; read and print it
		call 	PrintHex
		dcr 	l
		mov 	a,m 										; read it and fall through to print it.

; *******************************************************************************************************************
;
;									Print A as Hex Number, breaks ABCDE
;	
; *******************************************************************************************************************
PrintHex:
		mov 	b,a 										; save in B
		rrc 												; shift right 4
		rrc
		rrc
		rrc
		call 	_PHPrintHex 								; print it
		mov 	a,b 										; get LSB
_PHPrintHex:
		ani 	0Fh 										; lower 4 bits only
		cpi 	10 											; < 10, skip 
		jc 		_PHIsDigit
		adi 	7
_PHIsDigit:
		adi 	'0' 										; make ascii
		rst 	PrintCharacter 								; print it.
		ret

; *******************************************************************************************************************
;
;						Read string into Character Buffer, Breaks DEA, returns HL pointing to text
;
; *******************************************************************************************************************

ReadString:
		mvi 	a,12 										; clear display ready for input.
		rst 	PrintCharacter
ReadStringNoClear:
		mvi 	h,cursor/256 								; set the cursor to home
		mvi 	l,cursor&255 								; so we start from the beginning whatever is in the
		mvi 	m,0 										; buffer.
RSLoop:
		call 	RSSetHLCursor
		mvi 	m,']'										; put ] at cursor
		mov 	a,l 										; reached the end - 1 ??
		cpi 	31											; then exit.
		jz 		RSExit 										
		call 	RefreshScope 								; refresh the display.
		hlt 												; get character
		call	RSSetHLCursor
		mvi 	m,' '										; put space at cursor
		in 		6 											; read character.
		cpi 	13
		jz 		RSExit 										; if it is CR, return.
		cpi 	96 											; convert upper to lower case.
		jc 		RSNotLower
		sui 	32
RSNotLower:
		rst 	PrintCharacter 								; print character
		jmp 	RSLoop 
RSExit:	
		call 	RefreshScope 								; RefreshScope and exit.
		call 	RSSetHLCursor 								; HL now points to the cursor position.
		mvi 	m,0 										; make ASCIIZ string
		mov 	l,m 										; clear L, now points to start of string
		ret
;
;		Set HL to point to the cursor.
;
RSSetHLCursor:
		mvi 	h,Cursor/256 								; point HL to cursor
		mvi 	l,Cursor&255
		mov		l,m 										; HL now points to cursor
		ret

; *******************************************************************************************************************
;
;										Print String. Breaks DEA, HL points to zero
;
; *******************************************************************************************************************

PrintString:
		mov 	a,m 										; read character
		ora 	a 											; exit if zero via RefreshScope
		jz 		RefreshScope
		rst 	PrintCharacter 								; print character
		inr 	l 											; bump HL
		jnz 	PrintString
		inr 	h
		jmp 	PrintString

; *******************************************************************************************************************
;
;												Print Character. Breaks DEA
;
;	Understands 8 (Backspace) 9 (Tab) 12 (Clear) 13 (Carriage Return)
;	On CR or reaching end of the single line, it pauses for a key stroke then clears the screen.
; *******************************************************************************************************************

_PrintCharacter:
		mov 	e,l 										; copy HL to DE
		mov 	d,h
		mvi 	h,tempHL_PC/256 							; point HL to save area.
		mvi 	l,tempHL_PC&255
		mov 	m,e 										; save DE (originally HL)
		inr 	l	
		mov 	m,d
		mvi 	l,cursor & 255 								; point HL to cursor.

		cpi 	8 											; is it backspace ?
		jz 		_PCBackspace
		cpi 	9											; is it tab ?
		jz 		_PCTab 										
		cpi 	12 											; is it clear screen ?
		jz 		_PCClear
		cpi 	13 											; is it CR ?
		jz 		_PCReturn

		mov 	l,m 										; L now points to the character buffer cursor position
		mov 	m,a 										; save in character buffer at cursor position
		inr 	l 											; increment L - next character position.
		mov 	a,l 										; put in A
		mvi 	l,cursor & 255 								; point HL to cursor.
		mov 	m,a 										; write it back from A
		cpi 	32 											; done the whole line, do we need a key & clear
		jc 		PCExit 										; exit if we don't.
_PCReturn:
		call 	RefreshScope 								; update the screen.
		hlt 												; pause for a key to be pressed.
_PCClear:
		mvi 	m,0 										; home cursor
		mvi 	l,(characterBuffer+31) & 255 				; HL points to last character of buffer
_PCClearLoop:
		mvi 	m,' ' 										; fill buffer with spaces
		dcr 	l
		jp 		_PCClearLoop
		call 	RefreshScope 								; Refresh the screen, which is now clear.
PCExit:	mvi 	l,tempHL_PC & 255 							; point HL to save area
		jmp 	RestoreHLFromHL

_PCBackSpace:
		mov 	a,m 										; read cursor
		ora 	a 											; if zero, can't backspace
		jz 		PCExit
		sui 	1 											; back 1 character
		mov 	m,a
		mov 	l,a 										; HL points to character in buffer.
		mvi 	m,' '										; clear it as we've just backspaced
		jmp 	PCExit 										; and exit

_PCTab:	mov 	l,m 										; HL now points to cursor position.	
		mvi 	m,' '										; write space there.
		inr 	l 											; increment it
		mov 	a,l 										; new cursor position in A
		mvi 	l,cursor & 255 								; HL points to cursor
		mov 	m,a 										; update cursor
		ani 	7 											; jump back if not on a tab stop
		jnz 	_PCTab
		mov 	a,m 										; check if end.
		cpi 	32 
		jz 		_PCReturn
		jmp 	PCExit 										; no, exit.

; *******************************************************************************************************************
;
;								RefreshScope Screen from Character Buffer. Breaks DEA.
;
; *******************************************************************************************************************

SCWLoad = 	0Dh												; port allocation.
SCWWrite = 	0Eh
SCWOsc = 	10h
SCWData = 	11h

RefreshScope:
		mov 	e,l 										; copy HL to DE
		mov 	d,h
		mvi 	h,tempHL_RS/256 							; point HL to save area.
		mvi 	l,tempHL_RS&255
		mov 	m,e 										; save DE (originally HL)
		inr 	l	
		mov 	m,d

		mvi 	l,characterBuffer & 255						; point HL to character buffer.

		mvi 	d,32 										; characters to write.
		mvi 	a,040o 										; 00 100 000

		out 	SCWLoad 									; reset load
		out 	SCWData 									; output blank
		out 	SCWOsc										; output osc (osc on)
		out 	SCWWrite									; output write (Clear memory)

		mvi 	e,4 										; delay 150us
RFShortDelay1:
		dcr 	e
		jnz 	RFShortDelay1

		mvi 	a,1 										; output pb
		out 	SCWOsc
RFChar:
		mov 	a,m 										; output character
		out 	SCWData
		mvi 	a,1 										; pulse load
		out 	SCWLoad 	

		mvi 	e,4 										; delay 150us
RFShortDelay2:
		dcr 	e
		jnz 	RFShortDelay2
		xra  	a											; end pulse
		out 	SCWLoad
		inr 	l 											; advance pointer
		dcr 	d 											; do all 32 characters.
		jnz 	RFChar 

		mvi 	a,1 										; set re 
		out 	SCWWrite
		xra 	a 											; output osc 
		out 	SCWOsc
		mvi 	l,tempHL_RS & 255							; restore HL.
RestoreHLFromHL:
		mov 	e,m 										
		inr 	l
		mov 	h,m
		mov 	l,e
		ret

; *******************************************************************************************************************
;
;									Create vector jump table for RST8-38
;	
; *******************************************************************************************************************

PatchVectors:
		mvi 	b,7 										; patch all the reset vectors.

PVPatchVectors:
		mov 	a,b 										; get vector #
		add 	a 											; x 8
		add		a
		add 	a
		adi 	3 											; now points to default value
		mov 	l,a 										; make HL point to default value
		mvi 	h,0 
		mov 	e,m 										; copy default into DE
		inr 	l
		mov 	d,m

		mov 	a,b 										; get vector #
		add 	a 											; x 3
		add 	b
		adi 	(resetVectors-3) & 255 						; point HL to correct reset vectors
		mov 	l,a
		mvi 	h,resetVectors/256 	
		mvi 	m,044h 										; write $44 (JMP)
		inr 	l
		mov 	m,e 										; and the default vector
		inr 	l
		mov 	m,d
		dcr 	b 											; do for all vectors except RST $00 obviously
		jnz 	PVPatchVectors
		ret
;
;	A [xxxx] 	set address to xxxx
; 	D [xxxx] 	disassemble from address xxxx
;	L [xxxx] 	Assign xxxx to label 
;	I [xxxx] 	Inject byte xxxx using previous address and bump it.
;	S [xxxx] 	show from address and advance to byte after [xxxx:xx xx xx xx xx xx xx xx]
;	R [xxxx]	run from address xxxx
;	
;	(note, cannot use V and W because of the compressor)
;	
; 	aaa [xxxx] 	micro assembler of aaa with operand xxxx [assembled code e.g. xxxx:xx xx xx] and advance to byte after
;	(note RST, INP and OUT are different)
;
;	[x] = 		Label as a constant e.g. you can write 47= and the value will be label 47.
; 	