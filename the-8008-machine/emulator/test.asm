	
		cpu		8008new

		org 	2000h

		jmp 	2003h
		in 		1

		mvi 	h,20h
		mvi 	l,0

loop:	mov 	m,l
		inr 	l
		jnz 	loop
		inr 	h
		mov 	a,h
		cpi 	24h
		jnz 	loop

wait:	jmp 	wait

