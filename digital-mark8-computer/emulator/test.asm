	
		cpu		8008new
		org 	0

		mvi 	a,377o
		out 	14
		xra 	a
		out 	14
		mov 	b,a

loop1:	
		mov 	a,b
		ori 	200o
		out 	14
		xra 	a
		out 	14
		mov 	a,b
		ori 	200o
		out 	14
		xra 	a
		out 	14
		inr 	b
		jnz 	loop1

Wait:	in 		6
		ora 	a
		jp 		Wait
		ori 	200o
		out 	14
		xra 	a
		out 	14
		jmp 	Wait

