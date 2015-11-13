; ******************************************************************************************************************
;
;		Hardware test. 
;
;		Displays a checkerboard pattern, alternating pixels, then echos toggles to LEDs.
;		if A is pressed, shows ASCII keyboard input on LEDs.
;
; ******************************************************************************************************************

;
;	Make keyboard a -ve going strobe keyboard, and check ASCII returned.
;
		cpu	sc/mp

		org 	0x0000

		nop



		ldi 	0x0C
		xpah 	p1
		ldi 	0x41
		xpal 	p1

OutBit 	macro	advance
		xpah 	p3
		rrl
		xpah 	p3
		xpal 	p3
		st 		@advance(p1)
		rrl 	
		xpal 	p3
		endm

		ldi 	160
Loop2	xpal 	p2
		ldi 	0x7B
		xpah 	p3
		ldi 	0xCF
		xpal 	p3

		ldi 	0x10
Loop	xae
		OutBit	1
		OutBit	1	
		OutBit	30
		xae
		sr 
		jnz		Loop
		ld 		@-96(p1)
		ld 		@-64+4(p1)

		xpal 	p2
		scl
		cai 	1
		jnz 	Loop2

wait:	jmp 	wait