; **************************************************************************************************************
;									
;                                                   Fast Fill
;
; **************************************************************************************************************

	cpu 	8008new
	org 	0h
    jmp     Start

DHSetX      = 000o
DHSetY      = 100o
DHScanOff   = 201o
DHScanOn    = 202o
DHOnInc     = 204o
Display     = 010o

    org     080h

Start:
    mvi     a,DHScanOff                                             ; Display off
    out     Display

    mvi     a,DHSetX+0                                              ; Home to (0,0)
    out     Display
    mvi     a,DHSetY+0
    out     Display

    mvi     d,4096/256/4
    mvi     e,0
    mvi     a,DHOnInc
Out:
    out     Display
    out     Display
    out     Display
    out     Display
    dcr     e
    jnz     Out
    dcr     d
    jnz     Out
    hlt