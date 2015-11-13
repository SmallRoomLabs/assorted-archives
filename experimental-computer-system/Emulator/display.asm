; **************************************************************************************************************
; **************************************************************************************************************
;
;                   Character Display Routine and Font for Hogenson Oscilloscope Display
;                   ====================================================================
;
;   Note: Carl Helmers original code TTYOUT only breaks D ; preserving all the registers isn't possible without 
;         hardware add-ons a la Martin Research. Modifications to the code may be required because of this.
;
;   Self modifies both on the reload of HL at the end and in the storing of the X and Y drawing positions
;   which are immediate instructions at PCXPositionLoader and PCYPositionLoader
;
;   These routines (currently 248 bytes) should ideally be on a page on their own, alternatively the Character
;   Table can be moved to ROM.
;
; **************************************************************************************************************
; **************************************************************************************************************

    align 256

; **************************************************************************************************************
;
;                                       Print a Carriage Return / Line Feed
;   
; **************************************************************************************************************

PrintCRLF:
    mvi     a,0Dh                                               ; set A to 13 and drop through.

; **************************************************************************************************************
;
;                                       Print A - breaks DE, preserves BCHL. 
;
; **************************************************************************************************************

PrintCharacter:
    mov     d,h                                                 ; put HL into DE to save it
    mov     e,l
    mvi     l,(PCReloadRegisters+2) & 255                       ; point to the restore instructions
    mvi     h,(PCReloadRegisters+2) / 256   
    mov     m,d                                                 ; and change them to reload HL
    inr     l                                                   ; these are mov a,d mvi h,nn and mvi l,nn
    inr     l
    mov     m,e
    mov     d,a                                                 ; save character to print into D

    cpi     13                                                  ; is it a carriage return
    jz      PCNewLineCode                                       ; if so, do a carriage return.

PCYPositionLoader:
    mvi     e,50                                                ; fetch the Y coordinate into E
    ani     03Fh                                                ; convert to 6 bit ASCII
    add     a                                                   ; double it as words.
    adi     CharacterTable & 255                                ; now a pointer into character data
    mov     l,a                                                 ; because H has been set in the register saving code.
    mov     a,m                                                 ; read Low byte in
    inr     l
    mov     h,m                                                 ; high byte straight into H
    mov     l,a                                                 ; graphic data now in HL.

PCNextCharacterRow:

PCXPositionLoader:
    mvi     a,0                                                 ; fetch the X coordinate
    out     Display_Port
    mov     a,e                                                 ; get the Y coordinate
    ani     3Fh                                                 ; force into range 0-63
    ori     Display_SetY                                        ; make into a SET-Y command
    out     Display_Port

    call    NextBit                                             ; output the three bits
    call    NextBit
    call    NextBit
    inr     e                                                   ; next row down.
    mov     a,h                                                 ; finish when this is $80. 
    ora     l
    xri     080h                                                ; will be zero if was $80
    jnz     PCNextCharacterRow

    mvi     l,(PCXPositionLoader+1) & 255                       ; set HL to point to X position value 
    mvi     h,(PCXPositionLoader+1) / 256                       ; (H was changed by the drawing code)
    mov     a,m                                                 ; shift four to the lft.
    adi     4
    ani     3Fh
    mov     m,a                                                 ; write it back.
    jz      PCNewLineCode                                       ; if zero, do a new line.
PCReloadRegisters:
    mov     a,d                                                 ; restore character that was printed.
    mvi     h,0                                                 ; these are self modified.
    mvi     l,0 
    ret
;
;   New line, on entry H points to page.
;
PCNewLineCode:
    xra     a                                                   ; output the X writing position (LHS)
    out     Display_Port
    mvi     l,(PCYPositionLoader+1) & 255                       ; point HL to the Y Position Loader.
    mov     a,m                                                 ; read the Y position into A
    adi     6                                                   ; six lines down.
    cpi     58                                                  ; have we space for another line ?
    jc      _PCNoHomeRequired
    xra     a                                                   ; home to line 0
_PCNoHomeRequired:
    mov     m,a                                                 ; write it back.
    ori     Display_SetY                                        ; make it a display:setY instruction
    out     Display_Port
    mvi     e,6*64/2                                            ; number of writes to do to clear the next line.
    mvi     a,Display_ClearAdv
PCNewLineClear:
    out     Display_Port                                        ; doing it in twos is quicker and also use byte counter
    out     Display_Port
    dcr     e
    jnz     PCNewLineClear  
    mvi     l,(PCXPositionLoader+1) & 255                       ; zero the X position
    mov     m,e
    jmp     PCReloadRegisters                                   ; Reload registers and exit.
 
;   Output the top bit from HL to the display, shifting the whole thing left.
;
NextBit:
    mov     a,l                                                 ; shift L right into H
    add     a
    mov     l,a
    mov     a,h                                                 ; shift H right into Carry
    adc     a
    mov     h,a                                                 ; carry has bit to draw.
    mvi     a,0                                                 ; clear A
    sbi     (0-Display_ClearAdv) & 255                          ; make A into 204/205 using borrow because it's backwards.
    out     Display_Port
    ret

; *************************************************************************************************************
;
;                                 3 x 5 font, occupies 128 bytes of memory
;
; *************************************************************************************************************

    align   2                       ; must be on an even address as we just use INR L to get 2nd byte.
CharacterTable:
    include fonttable.asm

