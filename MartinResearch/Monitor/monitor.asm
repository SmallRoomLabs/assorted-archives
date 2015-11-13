; ************************************************************************************************************
;
;												MIKE-2 Monitor
;												==============
;
;	Rewritten by Paul Robson 21 August 2015 to specification given in Microcomputer Design and 
; 	Micro-8 volume 1 pp 170.
;
;	Paul Robson (paul@robsons.org.uk)
;
;	2) 	Debouncing ???
;
;	A/B/C 	General
;	D 		LSB determines whether code or data displayed.
; 	E 		Built up typed in data.
;	H 		H value
;	L 		L value
; 	Save1 	Temporary register / Save & Restore
; 	Save2	Displayed Data Value / Save & Restore.
;
; ************************************************************************************************************

		cpu 	8008new 							; done in newer 8008 mnemonics, much more readable :)

		org 	0h

RSTTable = 	0400h 									; Vectors for RST 8,10,18,20,28,30 start offset from 3 here.
													; e.g. RST 10 does jmp 0406h (Direct jump not indirect)

SaveMem = 	0FF8h 									; where registers are saved and reloaded.

SaveA = 	SaveMem+0 								; Defined by "Micro-8 volume 1 page 170"
SaveB = 	SaveMem+1
SaveC = 	SaveMem+2								; note that changing the order changes the actual code.
SaveD = 	SaveMem+3
SaveE = 	SaveMem+4
SaveFlags = Savemem+5
SaveH = 	SaveMem+6
SaveL = 	SaveMem+7

StartProgram = 0400h 								; where go goes 010 000 which is the first byte of RAM memory.

;
;	The Save ports have been moved because they collide with the 420 Device which uses 6/7.
;

InSave1  =  04h										; I/O Ports for latch # 1 
OutSave1 = 	InSave1+8
InSave2 =   05h 									; I/O Ports for latch # 2
OutSave2 = 	InSave2+8

;
;	Where the display ports are.
;
DisplayRight = 0Fh 									; Display Right (in Microcomputer Design and Micro-8)
DisplayMiddle = 0Eh 								; Display Middle 
DisplayLeft = 0Dh 									; Display Left

KeyboardPort = 06h 									; Keypad.

KeyContinue = 13									; Keypad Key assignments.
KeyGo = KeyContinue-1 								; If the order changes, will have to rethink the decoding code.
KeyLoadL = KeyContinue-2
KeyLoadH = KeyContinue-3
KeyRead = KeyContinue-4
KeyWrite = KeyContinue-5

; ************************************************************************************************************
;
;		Entry point for both first boot and interrupt. Using the hardware latches, *all* registers
;		are saved in memory.
;
; ************************************************************************************************************

EntryPoint:
	out 	OutSave1 								; save A in hardware#1
	mov 	a,l 									; save L in hardware#2
	out 	OutSave2
	mov 	a,h 									; copy H to A
	jmp 	ContinueEntry 							; skip over the exit load routine here.

	org 	8h
	jmp 	RSTTable+08h/8*3 						; LoadH/LoadL go in these 5 bytes
	org 	10h
	jmp 	RSTTable+10h/8*3 						; Read/Write memory in these 5 bytes
	org 	18h
	jmp 	RSTTable+18h/8*3 						; Register Restore tail code in these 5 bytes
	org 	20h
	jmp 	RSTTable+20h/8*3 						; (5 bytes free here)
	org 	28h
	jmp 	RSTTable+28h/8*3 						; (5 bytes free here)
	org 	30h
	jmp 	RSTTable+30h/8*3


; ************************************************************************************************************
;
;		Continue program option. Restores A,B,C,D,E,H,L and Flags, using external hardware, then executes
;		RET. If this is an interrupt, then this should resume (as the hardware jammed RST0 in).
;		If it is an initial boot, this should loop round, as the stack is cleared by RESET.	
;
;		It is also called as a subroutine by the GO command.
;
; ************************************************************************************************************

RestoreRegisters:
	mvi 	l,SaveA & 255 							; point HL to SaveA
	mvi 	h,SaveA / 256
	mov 	a,m 									; read saved A
	out 	OutSave1 								; save A in hardware #1
	inr 	l  										; point to B and read it.					
	mov 	b,m
	inr 	l  										; point to C and read it.					
	mov 	c,m
	inr 	l  										; point to D and read it.					
	mov 	d,m
	inr 	l  										; point to E and read it.					
	mov 	e,m
	inr 	l  										; point to Flags and read it.					
	mov 	a,m 
	out 	OutSave2 								; save Flags in hardware#2
	inr 	l 										; point to saved H
	mov 	a,m 									; read saved H into A
	inr 	l 										; point to saved L
	mov 	l,m 									; read saved L into L
	jmp 	CABComplete

SkipContinue:
	org 	1Bh 									; squeeze a bit of code into the RST space.
CABComplete:
	mov 	h,a 									; copy loaded H into H - now fixed up B,C,D,E,H,L
	in 		InSave2 								; read flags from hardware#2
	add 	a 										; and put them back 
	in  	InSave1 								; restore A from hardware#1
	ret 	

	org 	SkipContinue

; ************************************************************************************************************
;
;								Continuation of Entry (Saving Registers)
;
; ************************************************************************************************************

ContinueEntry:
	mvi 	l,SaveH & 255 							; point HL to H save point.
	mvi 	h,SaveH / 256
	mov 	m,a 									; save H there

	mvi 	a,0										; assume zero (reconstructing status register)
	jz 		EntryGetCarry							; if true, get carry bit
	mvi 	a,18h 									; assume positive
	jp		EntryGetCarry 							; if true, get carry bit
	mvi 	a,0C0h 									; sign flag was set
EntryGetCarry:
	rar 											; get carry bit.
	jpe 	ParityEven 								; if the parity is odd.
	xri 	1										; fix it up so the new parity will be odd.
ParityEven:	

	dcr 	l 										; point to SaveFlags
	mov 	m,a 
	dcr 	l 										; point to E
	mov 	m,e
	dcr 	l 										; point to D
	mov 	m,d
	dcr 	l 										; point to C
	mov 	m,c
	dcr 	l 										; point to B
	mov 	m,b
	dcr 	l 										; point to A
	in 		InSave1 								; fetch saved A from hardware #1
	mov 	m,a 

	mvi 	l,SaveL & 255 							; point HL to Save L
	in 		InSave2 								; get saved L from hardware #2
	mov 	m,a 									; and save L.

	mvi 	h,0 									; clear HL (current address)
	mov 	l,h 						

; ************************************************************************************************************
;
;									Loop Back here to load E with (HL)
;
; ************************************************************************************************************

ReadMemoryUpdateDisplay:
	mov 	e,m 									; E now contains the contents of memory.
	out 	OutSave2 								; Save it in OutSave 2 (displayed data value)
	mvi 	e,0 									; clear E (built up value)

; ************************************************************************************************************
;
;									Update Display and show data first
;
; ************************************************************************************************************

UpdateDisplayShowDataFirst:
	mvi 	d,1 									; Set D LSB, so that after increment D C is clear so shows 0:E

; ************************************************************************************************************
;
;					Update the Display, depending on the LSB of D (1 = Data,0 = Address)
;
; ************************************************************************************************************

UpdateDisplay:
	mvi 	b,0 									; set B to zero, the upper octal digits.
	inr 	d 										; toggle bit 0 of D.
	mov 	a,d 									; put D in A
	rrc 											; put bit 0 of D in the carry.
	in 		InSave2 								; read the lower byte when displaying data.
	jnc 	UpdateDisplayExitBA						; carry clear show 0:(S2), carry set show H:L.
	mov 	b,h 									; set BA to the value in HL.
	mov 	a,l 

; ************************************************************************************************************
;
; 			Update the display , at this point A contains the low byte, and B the high byte.
;
; ************************************************************************************************************

UpdateDisplayExitBA:
	out 	OutSave1 								; save the LSB in hardware#1
	ani 	7 										; this is the lowest octal digit we want
	mov 	c,a 									; put that low digit in C
	in 		InSave1 								; get the original value back.
	ani 	38h 									; get the middle octal digit.
	add 	a 										; shift if left - now contains middle octal digit in .xxx ....
	ora 	c 										; A now contains .xxx .xxx 
	out 	DisplayRight 	

	mov 	a,b 									; Get the high byte 
	ani 	7 										; this is the least significant digit of the msb, goes in middle upper
	add 	a 										; A now contains .... xxx.
	add 	a 										; A now contains ...x xx..
	mov 	c,a 									; save in C
	in 		InSave1 								; Get the original low byte back.
	ani 	0C0h 									; then isolate the last octal digit, the upper 2. dd.. ....
	ora 	c 										; A now contains dd.x xx..
	rrc 											; A now contains d.xx x..d
	rrc												; A now contains .xxx ..dd (there are only 2 bits in upper octal 8 bits)
	out 	DisplayMiddle 							; put it on the middle display.

	mov 	a,b 									; get the high byte
	ani 	38h 									; isolate its middle octal digit ..xx x...
	rar 											; now contains ...x xx.. 
	mov 	c,a 									; C now contains ...x xx..
	mov 	a,b 									; get the high byte
	ani 	0C0h 									; isolate its upper octal digit yy.. ....
	ora 	c 										; now we have yy.x xx..
	rrc 											; and now .yy. xxx.
	rrc 											; and finally, ..yy .xxx - which is the left hand display
	out 	DisplayLeft 							; so put it there.

; ************************************************************************************************************
;
;		Wait for key to be released.
;
; ************************************************************************************************************

WaitNoKeyPressed:									; wait until the keyboard is all clear.
	in 		KeyboardPort 							; Read the keyboard port
	jnz 	WaitNoKeyPressed 						; there are keys pressed. 

; ************************************************************************************************************
;
;		Wait for key press, if time out then switch display. 
; 		The testing loop takes 5+9+5+11+5+11 = 46 cycles.
;		Toggles at 1.2s
;		The CPU is clocked at 400Khz, 2.5 microseconds, therefore a total of 480,000 cycles between updates.
;		480,000 / 46 = 10,434 which is the number of times we want to execute the loop.
; 		10,434 / 256 = Outer C loop = 40.7 iterations.
;
; ************************************************************************************************************
	
	mvi 	b,41 									; Will average 41*256+128 = 10,624 iterations = 1.22s approx.
													; not worth initialising C, we don't need that precision.
WaitKeyPressed:
	dcr 	c 										; Outer loop [5 cycles]
	jnz 	NoDelayCheck 							; Normally this is taken [11 Cycles] [] are what usually happens.
	dcr 	b 										; Decrement B [5 cycles]
	jz 		UpdateDisplay 							; If reached zero, switch display [9 cycles]
NoDelayCheck:

	in 		KeyboardPort 							; Wait for a key press [5 cycles]
	jz 		WaitKeyPressed 							; Loop Back if Clear [11 cycles]

	cpi 	8 										; control keys are 8 up. Write/Read/LoadH/LoadL/Go/Continue ?
	jp 		ControlKey 								; its a control key.

	mov 	b,a 									; save in B
	mov 	a,e 									; retrieve E value which is aabb bccc
	add 	a 										; shift left three times.
	add 	a
	add 	a 										; so now it is bbcc c...
	ora		b 										; or the new value in bbcc cxxx
	mov 	e,a 									; update E
	out 	OutSave2 								; and now display this as well.
	jmp 	UpdateDisplayShowDataFirst 				; and show the 0E first rather than the address.

; ************************************************************************************************************
;
;										Process Control Keys. 
;
; ************************************************************************************************************

ControlKey:
	adi 	256-KeyContinue 						; A will be zero if it is Continue
	jz 		RestoreRegisters 						; So if it is, go to the return from interrupt code.
	mov 	b,a 									; So we can use INR to test it.
	inr 	b 										; A will be zero if it is Go.
	jz 		StartProgram							; Run Program.

	cm 		ExecuteControl 							; Execute Control in A, only if bit 7 set.
	jmp		ReadMemoryUpdateDisplay 				; Read Memory, Update and Display Data

; ************************************************************************************************************
;
;	Execute Control. At this point $FF:Load L $FE:Load H $FD:Read $FC:Write can also be $FB..$80 ignore these.
;
;	Returning from this subroutine updates E with (HL)
;
;	The subroutine allows us to replace several JMPs with RET. 
;
; ************************************************************************************************************

ExecuteControl:
	inr 	b 										; update B, now zero if LoadL
	jz 		CtrlLoadL
	inr 	b 										; update B, now zero if LoadH.
	jz 		CtrlLoadH 								

	inr 	b 										; update B, now $00 if Read $FF if write
	inr 	b 										; update B, now $01 if Read $00 if write
	rm 												; if signed, it's a bad command, ignore it.
	jz 		WriteMemory 							; if zero (write) then write to memory.
	jmp 	ReadMemory 								; otherwise, read memory.

; ************************************************************************************************************
;
;		Memory read/write : read bumps HL, and reads the next byte for display, clearing E.
;							write does the same but copies E into memory first
;
; ************************************************************************************************************

	org 	13h										; code fits in the gaps between RSTxx

WriteMemory:										; write does a write then increment then read.
	mov 	m,e 									; write out and increment address LSB in L
ReadMemory:											; read does an increment and read.
	inr 	l 					
	rnz 											; no carry out, so return which does the read
	inr 	h 										; increment address MSB in H
	ret

; ************************************************************************************************************
;
;				Load H/Load L : copies E to H/L, reads next byte for display, clearing E.
;
; ************************************************************************************************************

	org 	0Bh 									; fits in the RSTxx gaps.

CtrlLoadL:
	mov 	l,e 									; copy E into L and display data there
	ret

CtrlLoadH:
	mov 	h,e 									; copy E into H and display data there
	ret
