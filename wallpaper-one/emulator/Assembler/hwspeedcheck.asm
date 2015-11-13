
		cpu	sc/mp

		org 	0x0000
		nop
		ldi 	0x8
		xpah 	p1
		ldi 	0x0
		xpal 	p1
		ldi	 	0x0
		st 		0(p1)
		ldi 	0
		st 		1(p1)

DoStuff:
		ld 		@2(p2)
		ani 	7
		dad 	0(p1)
		cai 	DoStuff
		xae
		ccl
		dint
		csa
		cas
		xpah 	p3
		dld		0(p1)
		jnz		DoStuff
		dld		1(p1)
		st 		0
		jnz 	DoStuff

		ldi 	0x88
		st 		0


Wait:	jmp 	Wait