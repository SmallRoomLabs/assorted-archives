; ******************************************************************************************************************
;
;													Speed Test
;
; ******************************************************************************************************************

		cpu	sc/mp

		org 	0x0000

		nop

		ldi 	0x8
		xpah 	p2
		ldi 	0x00
		xpal 	p2
		ldi 	0x00
		st 		0(p2)
		st 		1(p2)
		st 		2(p2)
		jmp 	loop2

	loop1:
		ild 	0(p2)
		jnz 	loop1
		ild 	1(p2)
		jnz 	loop1
		ild 	2(p2)
		st 		0
		jmp 	loop1

;
;		Without DLY 255 (loop1)
;	
;		The classic inner loop:
;			ild 0(p2)		22 cycles
;			jnz loop 		11 cycles (successful)
; 			Total 			33 cycles
;
;		Two inner loops are executed 65536 times 
;	
;		65536 * 33 = 2,162,688
;
;		Running at 2Mhz, each tick should take 2.16 seconds.
;
;		Counting to 16 took 33.6 s 
; 		16 * 2.16 = 34.56s , near enough allowing for measuring errors.			
;

	loop2:
		dly 	255
		ld 		0(p2)
		ccl
		adi 	32
		st 		0(p2)
		jnz 	loop2
		ild 	1(p2)
		st 		0
		jmp 	loop2
;
;		With DLY 255 (loop2)
;
;		The DLY 255, executed 8 times should take approximately
;
;		255 * 514 * 8 cycles = 1,048,560 cycles
;
;		1:06 mins to count to 64 i.e. near enough 
;