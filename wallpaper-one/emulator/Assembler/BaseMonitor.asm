; ******************************************************************************************************************
;
;												Base Machine Monitor
;												====================
;
;							Uses only LEDs or 7 segment display, toggle switches, and A/B/Reset
;
;	Functionality: (RAM reading+writing from $0800-$08FF only, run code at $0800)
;
;		Key A. 	 Copies toggles as address. Shows address until released. 
;				 If toggles are 0x00 then it increments rather than changes address.
;
;		Key B. 	 Copies data to address, advance address. Show address until released.
;				 When neither key pressed shows data.
;
;		Key RST. Restarts, zeroes address. 
;				 Reset with A pressed runs from $800
;
; ******************************************************************************************************************

		cpu	sc/mp

SenseA 	= 0x10															; masks for A + B
SenseB 	= 0x20
E 		= -0x80															; read memory via E.

FlashSpeed = 155

		org 	0x0000

		nop
		dint 															; disable interrupt.

		ldi 	0x08													; P1/E is the current address.
		xpah 	p1
		ldi 	0x00
		xpal 	p1

		ldi 	0x0C 													; P2 points to toggle switches.
		xpah 	p2
		ldi 	0x00
		xpal 	p2

		csa 															; See if we are running code
		ani 	SenseA
		jnz 	-1(p1) 													; if A pressed run from $800


		ldi 	0x00 													; set address to 00

UpdateAddress:		
		xae

EndCommand:
		lde 															; copy E (address) to LEDs
		st 		0 	
WaitRelease:
		csa 															; wait for SA + SB to be released
		ani 	SenseA + SenseB 	
		jnz 	WaitRelease

		ld 		E(p1)													; read data at address and display on toggles.
		st 		0

WaitCommand:															; wait for SA or SB to be pressed.
		csa
		ani 	SenseA + SenseB 	
		jz 		WaitCommand
		ani 	SenseA 
		jnz 	PressedAKey

PressedBKey:
		ld 		0(p2) 													; read toggles
		st 		E(p1) 													; write to current address
		jmp 	IncrementAddress 										; go to next address

PressedAKey:	
		ld 		0(p2) 													; read toggles.
		jnz 	UpdateAddress 											; if non-zero, use that address.

IncrementAddress:
		ccl																; add 1 to E, incrementing.
		ldi 	1
		ade  	
		jmp		UpdateAddress
