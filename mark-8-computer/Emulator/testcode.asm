
	cpu 	8008new

    org     0

    mvi     h,0
    mov     l,h
    hlt
    in      0
wait:
    jmp     wait
