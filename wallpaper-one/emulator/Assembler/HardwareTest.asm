; ******************************************************************************************************************
;
;		Hardware test. 
;
;		Displays a checkerboard pattern, alternating pixels, then echos toggles to LEDs.
;		if A is pressed, shows ASCII keyboard input on LEDs.
;
; ******************************************************************************************************************

		cpu	sc/mp

		org 	0x0000

		nop

		ldi 	0x00
		xpal 	p1
		ldi 	0x0C ! 0x10
checkerboard:
		xri		0x10
		xpah 	p1

		lde 								; xor bit 7 and bit 2.
		jp 		_Bit7Clear
		xri 	04 							; toggle bit 2.
_Bit7Clear:
		rr 									; shift to bit 0.
		rr
		st 		@1(p1) 						; write to video display

		ldi 	1 							; increment E.
		ccl
		ade
		xae 

		xpah 	p1
		xri 	0x10
		jnz 	checkerboard

loop:	
		ldi 	0x0C 						; point to input area
		xpah 	p1
		csa 								; read SA
		ani 	0x10
		jz 		_Toggles 					; skip if not pressed
		ldi 	1
_Toggles:
		xpal 	p1
		ld 		(p1) 						; read switches and store them to the LEDs.
		st 		0
		
		jmp 	loop		
