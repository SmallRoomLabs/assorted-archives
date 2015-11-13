	
		cpu		8008new

		org 	0
		mvi 	h,03Ch
		mvi 	l,0
fill:	mov 	m,l
		inr 	l
		jnz 	fill
		inr 	h
		mov 	a,h
		cpi 	040h
		jnz 	fill
		
		mvi 	h,3Ch
		mvi 	l,0
loop:
		mov 	a,a
		hlt
		in 		0
		mov 	m,a
		out 	8
		inr 	l
		jmp 	loop
		