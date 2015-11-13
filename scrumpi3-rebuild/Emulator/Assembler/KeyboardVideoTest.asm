; *****************************************************************************************************************************************
;
;		Simple Keyboard/Video Test program. Displays keyboard scan rows at the top, full character set at the bottom
;
;		S3 displayed on RHS, also an ever changing graphic to show still functioning.
;
; *****************************************************************************************************************************************

		cpu	sc/mp
		nop

		ldi 	0x0E												; Set P1.H to video
		xpah 	p1
		ldi 	0 													; P1.L will be in E
ClearLoop:
		xae 														; set P1.L
		lde
		xpal 	p1
		ldi 	0x20 												; write space
		st 		0(p1)
		lde 														; skip if bottom half of screen
		jp 		_Not2ndHalf
		ccl															; shift bottom half left, so bit 6-> bit 7
		ade
		ani 	0x80 												; isolate bit 7
		xre 														; xor in data
		st 		0(p1)												; update it
_Not2ndHalf:
		lde 														; next char
		ccl
		adi 	1
		jnz 	ClearLoop 											; until screen done

Restart:
		ldi 	0xC													; P2 = C00 Keyboard port
		xpah 	p2
		ldi 	00
		xpal 	p2
		ldi 	8 													; E = 08 (keyboard read bit)
		xae
		ldi 	0x0E 												; P1 = Video RAM
		xpah 	p1
		ldi 	0x0
		xpal 	p1
		ldi 	'S'													; put S3 in top corner.
		st 		30(p1)
		ldi 	'3'
		st 		31(p1)
		ild 	95(p1) 												; puts activity so know hasn't crashed.
ReadKey:
		lde 														; write keyboard bit on left side
		ori 	'0'
		st 		0(p1)

		ld 		-0x80(p2)											; read keyboard row
		ani 	0xF 												; convert LSB to ASCII
		ccl
		dai 	0x90
		dai 	0x40
		st 		5(p1)												; write to screen

		ld 		-0x80(p2)											; do it again for MSB
		sr
		sr
		sr
		sr
		ccl
		dai 	0x90
		dai 	0x40
		st 		4(p1)

		ld 		@32(p1)												; advance next screen line

		sio 														; shift E right
		lde
		ani 	0x7F 												; lower 7 bits zero (SIN unknown)
		jnz 	ReadKey 											; next key line
		jmp	 	Restart
