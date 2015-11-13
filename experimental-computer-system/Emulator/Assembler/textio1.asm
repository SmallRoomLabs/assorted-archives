; **************************************************************************************************************
;
;                                         Screen I/O Functions									
;
; **************************************************************************************************************

	cpu 	8008new

	org 	0h
    mvi     a,201o
    out     8

    call    ClearScreen
    mvi     h,counter/256
    mvi     l,counter&255
    mvi     m,32
Next:
    mvi     h,counter/256
    mvi     l,counter&255
    mov     a,m
    adi     1
    cpi     160
    jz      Exit
    mov     m,a
    call    PrintCharacter
    jmp     Next
Exit:
    hlt

Counter:
    db      0

CharBuffer:
    db      0,0
ScreenPosition:
    db      0,0

; *************************************************************************************************************
;
;                                           Print a carriage return
;
; *************************************************************************************************************

PrintCRLF:  
    mvi     a,0Dh                   ; load CR (13) and fall through

; *************************************************************************************************************
;
;                                   Print character A at current position
;
; *************************************************************************************************************

PrintCharacter:
    mvi     h,CharBuffer/256        ; point HL to the buffer
    mvi     l,CharBuffer&255
    mov     d,h                     ; copy it into DE
    mov     e,l
    mov     m,a                     ; set buffer[0] to character
    inr     l                       ; set buffer[1] to zero binary.
    mvi     m,0                     ; and fall through

; *************************************************************************************************************
;
;   Print ASCIIZ string at DE at current position. Understands 13 (CR)
;   12 (Clear Screen) 1 (Home Cursor)
;
; *************************************************************************************************************

PrintString:
    mvi     h,ScreenPosition / 256  ; read screen position into BC
    mvi     l,ScreenPosition & 255
    mov     b,m
    inr     l
    mov     c,m

; *************************************************************************************************************
;
;   Print ASCIIZ string at DE at coordinates BC.
;   On exit BC points to new position, DE to ending character, Breaks HL.
;
; *************************************************************************************************************

PrintStringBC:

PrintStringLoop:
    mov     l,e                     ; copy DE -> HL
    mov     h,d 
    mov     a,m                     ; read character, first check for controls and EOL.
;
;   Handle 13 (CR) 1 (Home) 12 (Clear)
;
    cpi     13                      ; if CR
    jz      CarriageReturn          ; do the CR code.
    cpi     1                       
    cz      HomeCursor              ; if 1, home cursor (does not affect A)
    cpi     12
    cz      ClearScreen             ; if 12, clear screen (changes AHL but does not matter)

    mov     a,b                     ; will it fit on the line ?
    cpi     61                      ; e.g. after position 60
    jc      NoNextLine

CarriageReturn:
    mov     a,c                     ; down to next line with wrapping
    adi     6
    cpi     60                      ; will it fit on this line ?
    jc      NoTopOfScreen
    xra     a                       ; back to top of screen.
NoTopOfScreen:
    mov     c,a
    ori     100o                    ; position there vertically
    out     8
    xra     a                       ; left hand side horizontally.
    out     8
    mvi     b,64*5/2                ; number of required clears.
    mvi     a,205o                  ; clear and increment
ClearNextLine:
    out     8                       ; quicker and means only one counter.
    out     8
    dcr     b
    jnz     ClearNextLine
    mvi     b,0                     ; left hand side of screen. 
;
;   Now handle normal characters
;
NoNextLine:
    mov     l,e                     ; re-fetch character
    mov     h,d
    mov     a,m
    ora     a                       ; if zero set cursor and exit.
    jz      SetCursor               
    cpi     32                      ; control character ?
    jc      AdvanceNextCharacter    ; don't print it.     
    ani     03Fh                    ; make 6 bit ASCII.
    add     a                       ; double A
    adi     CharacterTable & 255    ; A now points into table
    mov     l,a                     ; Make HL point to the graphic data
    mvi     a,CharacterTable/256    
    aci     0
    mov     h,a

    mov     a,m                     ; A is the Graphic.Low
    inr     l                       ; point to Graphic.High
    mov     h,m                     ; Now make HL contain the graphic data
    mov     l,a 
;
;   Come back here for a new line
;
NewLine:
    mov     a,b                     ; output coordinates, x = b
    out     8
    mov     a,c                     ; y = c
    ori     100o                    ; make it a 1xx command
    out     8

    call    NextBit                 ; draw three horizontal bits.
    call    NextBit                 ; B is unchanged by this.
    call    NextBit
    inr     c                       ; next line down vertically.
    mov     a,h                     ; check if finished.
    ora     l
    xri     80h                     ; if all that's left is $80 - this is the MSB
    jnz     NewLine                 ; may be a point where certain fonts won't draw if DE = $8080 ?

    mov     a,b                     ; B along 4 (next character position)
    adi     4
    mov     b,a
    mov     a,c                     ; C back up 5, drawn 5 lines.
    sui     5
    mov     c,a
;
;   Advance DE pointer to next and go round again.
;
AdvanceNextCharacter:
    inr     e                       ; advance to next character
    jnz     PrintStringLoop
    inr     d
    jmp     PrintStringLoop
;
;   Output one row of bits
;
NextBit:
    mov     a,l                     ; shift L right into H
    add     a
    mov     l,a
    mov     a,h
    adc     a
    mov     h,a                     ; carry has bit to draw.
    mvi     a,0                     ; clear A
    sbi     173o                    ; make A into 204/205 using borrow because it's backwards.
    out     8
    ret

; *************************************************************************************************************
;
;                                           Clear Screen (breaks HL)
;
; *************************************************************************************************************

ClearScreen:
    xra     a                       ; set position on screen to (0,0)
    out     8
    mov     l,a                     ; L = 0
    mvi     a,100o
    out     8
    mvi     a,205o                  ; clear and advance.
    mvi     h,64*64/256/4           ; number to write.
ClearScreenLoop:
    out     8
    out     8
    out     8
    out     8
    dcr     l
    jnz     ClearScreenLoop
    dcr     h
    jnz     ClearScreenLoop         ; fall through to home cursor

; *************************************************************************************************************
;
;                                               Home Cursor (breaks HL)
;
; *************************************************************************************************************

HomeCursor:
    mvi     b,0                     ; set B = C = 0 and fall through.
    mov     c,b

; *************************************************************************************************************
;
;                                     Set cursor position to BC (breaks HL)
;
; *************************************************************************************************************

SetCursor:
    mvi     h,ScreenPosition / 256  ; write screen position from BC
    mvi     l,ScreenPosition & 255
    mov     m,b
    inr     l
    mov     m,c
    ret

; *************************************************************************************************************
;
;                                 3 x 5 font, occupies 128 bytes of memory
;
; *************************************************************************************************************

    align   2                       ; must be on an even address as we just use INR L to get 2nd byte.
CharacterTable:
    include fonttable.asm

