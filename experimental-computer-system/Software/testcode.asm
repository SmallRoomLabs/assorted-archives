; **************************************************************************************************************
;
;                                         "El Dump" (ECS 1-1)
;
; **************************************************************************************************************

	cpu 	8008new

Display_Port = 8
Display_SetY = 100o
Display_ScreenOff = 201o
Display_Set = 202o
Display_Clear = 203o
Display_SetAdv = 204o
Display_ClearAdv = 205o
Display_ScreenOn = 207o

Type    macro
    rst     30o
    endm
OctOut  macro
    rst     40o
    endm


	org 	011o*64

EDStart:
    mvi     a,2                                                 ; turn interrupts off
    in      0   

EDElDumpo:
    mvi     h,Counter/256                                       ; point HL to counter
    mvi     l,Counter & 255 
    mov     c,m                                                 ; dec counter, zero if exit
    dcr     c 
    jz      EDExit
    mov     m,c                                                 ; write it back

;    mov    a,a                                                 ; three NOPs for TTY.
;    mov    a,a
;    mov    a,a

    mvi     l,MemoryAddress & 255                               ; point HL to memory address
    mov     b,m                                                 ; load into BC.
    inr     l
    mov     c,m
    inr     c                                                   ; go to next memory address
    mov     m,c                                                 ; write it back.
    jnz     EDNoOverflow
    inr     b                                                   ; overflow, increment and write back
EDNoOverflow:
    dcr     l
    mov     m,b
    mvi     a,0Dh                                               ; print carriage return
    call    PrintCharacter
    mvi     a,0Ah                                               ; print line feed
    call    PrintCharacter

    mvi     l,2                                                 ; note, using L here.
    call    EDSpaces                                            ; draw 10 spaces.
    mov     a,b
    call    EDTByteOctal                                        ; print MSB
    mvi     a,'\\'                                              ; print seperator
    Type
    mov     a,c                                                 ; print LSB
    call    EDTByteOctal
    mvi     a,'='                                               ; print equals
    Type
    mvi     a, ' '                                              ; print space.
    Type
    mov     h,b                                                 ; address into HL
    mov     l,c
    mov     a,m                                                 ; read it
    call    EDTByteOctal
    in      1                                                   ; scan keyboard
    cpi     377o                                                ; no key pressed
    jz      EDElDumpo                                           ; go back if no key pressed

EDExit:
    hlt

EDTByteOctal:
    mov     l,a 
    rlc     
    rlc     
    ani     3
    OctOut
    mov     a,l
    rrc
    rrc
    rrc
    OctOut
    mov     a,l
    OctOut
    ret

EDSpaces:
    mvi     a,' '
    Type
    dcr     l
    jnz     EDSpaces
    ret

    org     0
    jmp     EDStart


MemoryAddress = 6
Counter = 5

    org     030o
    jmp     PrintCharacter

    org     040o
OctOut:
    ani     7
    ori     '0'
    rst     30o
    ret 


    include display.asm