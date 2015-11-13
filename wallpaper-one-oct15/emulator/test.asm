; ******************************************************************************************************************
;
;											8 digit counter, Q+D	
;
; ******************************************************************************************************************

		cpu	sc/mp

		org 	0x0000
		nop
		ldi 	0
		xpah 	p3
		ldi 	0
		xpal 	p3

		ldi 	0xC
		xpah 	p2

loop:	ld 		0(p2)
		jp 		loop
		xae 
		lde
		ani 	0x40
		jz		_nolc
		lde
		scl	
		cai 	0x20
		xae
_nolc:	lde
		ani 	0x3F
		st 		@1(p3)
		jmp 	loop
