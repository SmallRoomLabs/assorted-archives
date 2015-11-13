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
		ldi 	0
		xpal 	p2
		ldi 	total/256
		xpah 	p2
		ldi 	0
		st 		0(p2)		
		st 		1(p2)		
		st 		2(p2)		
		st 		3(p2)		
		st 		4(p2)		
		st 		5(p2)		
		st 		6(p2)		
		st 		7(p2)		

newdisplay:
		ldi 	0
		xpah 	p3
		ldi 	8
		xpal 	p3
		ldi 	8
display:
		xpal 	p2
		ld 		@-1(p2)
		ori 	48
		st		@-1(p3)
		xpal 	p2
		jnz 	display

		ldi 	7
		xpal 	p2
increment:
		ild 	0(p2)
		xri 	10
		jnz 	newdisplay
		st 		0(p2)
		ld 		@-1(p2)
		jmp 	increment

wait:	jmp 	wait


total = 0x800


