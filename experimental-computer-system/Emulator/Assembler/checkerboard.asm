; **************************************************************************************************************
;									
;                                      Checkerboard on Hogenson Display
;
;   From ECS Volume 1 Number 5.
;
; **************************************************************************************************************

	cpu 	8008new
	org 	0h
    jmp     Start

DHSetX      = 000o
DHSetY      = 100o
DHScanOff   = 201o
DHScanOn    = 202o
Display     = 010o

    org     080h

Start:
    mvi     a,DHScanOff                                             ; Display off
    out     Display

    mvi     a,DHSetX+0                                              ; Home to (0,0)
    out     Display
    mvi     a,DHSetY+0
    out     Display

    mvi     b,0                                                     ; Clear registers
    mov     c,b
    mov     d,b
    mov     h,b
    mov     l,b
    mvi     e,0
DecLoop:
    inr     e
    mov     a,e
    ani     1
    jz      DecLoopJump

    mvi     l,zni
DotLoop:
    mov     a,b
    sui     020o
    jz      XSecLoop

    inr     b
    mov     a,m
    out     Display
    jmp     DotLoop
DecLoopJump:
    mvi     l,zfi
    jmp     DotLoop

XSecLoop:
    mvi     b,0
    mov     a,c
    sui     3
    jz      RowLoop
    inr     c
    jmp     DecLoop

RowLoop:
    mvi     c,0
    mov     a,d
    ani     037o
    sui     017o
    jz      YSecLoop
    inr     d
    jmp     DecLoop

YSecLoop:
    mov     a,d
    ani     0340o
    mov     d,a
    sui     0140o
    jz      EndProgram

    mov     a,d
    adi     040o
    mov     d,a
    inr     e
    jmp     DecLoop
EndProgram:
    mvi     a,DHScanOn
    out     Display
    hlt 

zni:db  204o
zfi:db  205o

