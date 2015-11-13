uartPort = 0x7D00                                   ; UART is at $7D00
;
;       110 Baud TTY Speed Calculation
;
;       110 Baud = 110Hz.
;       CPU Cycles = 3.51Mhz / 2
;       110 Baud delay is 3,510,000/110 = 31,909 Cycles.
;       Delay time is (31909-13)/514 which is exactly 62.
;
baud110Delay = 62                                                       ; required value for 110 baud delay.

;
;       UART code is completely speculative. The only thing we actually know is that it is an AY-5-1013. This
;       chip has no fixed registers but it does have three access pins. 
;
;       Pin 4 : Received Data Enable is a logic 0 activated port read. I have connected this to A0.
;       Pin 23 : Data Strobe is a logic 0 activated port write. I have connected this to A1.
;       Pin 16 : Status word Emable is a logic 0 activated status read, I have connected this to A2.
;
uartPortBase = uartPort + 240                                           ; base address for R/W

uartReceivedData = 14                                                   ; (e.g. bit 0 only low)
uartDataStrobe = 13                                                     ; (e.g. bit 1 only low)
uartStatusWordEnabled = 11                                              ; (e.g. bit 2 only low)

uartSWEParityError = 1                                                  ; these are the connected status word bits
uartSWEFramingError = 2                                                 ; these are just errors.
uartSWEOverRunError = 4

uartSWEDataAvailable = 8                                                ; logic '1' when data is available to receive.
uartSWETransmitBufferEmpty = 16                                         ; logic '1' when data can be transmitted.

; *******************************************************************************************************************************
;
;                         Put A to Flag 1 as a 110 Baud TTY (1 Start, 8 Data, nothing else), preserves P1,P2
;
; *******************************************************************************************************************************

PutTTY:
        xae                                                             ; put in E
        ldi     10                                                      ; set bit count to 10. Start + 8 Data + Clearing value.
        st      -1(p2)                                                  ; counter is on stack.
_PTTYSetLoop:
        csa                                                             ; output start bit.
        ori     0x02 
_PTTYLoop:
        cas                                                             ; write A to S.
        ldi     0x00                                                    ; delay 110 baud.
        dly     baud110delay                                            
        dld     -1(p2)                                                  ; decrement the counter
        jz      _PTTYExit                                               ; and exit if it is zero.
        lde                                                             ; shift E left into the carry/link bit.
        ccl
        ade
        xae      
        csa                                                             ; get the status register, CY/L is bit 7.
        ani     0xFD                                                    ; clear the F1 bit, just in case.
        jp      _PTTYLoop                                               ; if it is '1' then output to S and delay
        jmp     _PTTYSetLoop                                            ; otherwise set it to '1' and delay.
_PTTYExit:
        xppc    p3                                                      ; exit, this is re-entrant.
        jmp    	PutTTY

; *******************************************************************************************************************************
;
;                              Read SB as a 110 Baud TTY into A (1 start bit, 8 data bits), preserves P1
;
; *******************************************************************************************************************************

GetTTY:
        ldi     0                                                       ; clear final result in E
        xae
_GTTYWait:
        csa                                                             ; wait until SB is logic '1', the start bit.
        ani     0x20                    
        jz      _GTTYWait                                               ; done !
        dly     baud110delay/2                                          ; go to middle of start pulse.
        ldi     8                                                       ; read this many bits.
        st      -1(p2)
_GTTYLoop:
        ldi     0                                                       ; go to the middle of the next pulse.
        dly     baud110delay
        csa                                                             ; read it in.
        ani     0x20                                                    ; mask out SB.
        jz      _GTTYSkipSet
        ldi     0x1 
_GTTYSkipSet:                                                           ; it is now 0 or 1.
        ccl
        ade                                                             ; E = E * 2 + A (e.g. shift the bit in.)
        ade
        xae                                                             ; put it back in E
        dld     -1(p2)                                                  ; do it 8 times
        jnz     _GTTYLoop                                       
        dly     baud110delay*5/2                                        ; ignore any stop bits and allow short delay
        xae                                                             ; get the result
        xppc    p3                                                      ; return
        jmp     GetTTY          	                                    ; get the TTY

; *******************************************************************************************************************************
;
;                               Read a byte from the UART to A. On exit CY/L => error, , preserves P1,P2
;
; *******************************************************************************************************************************

GetART: st 		@-1(p2) 												; space for the result
        ldi     uartPortBase / 256			                            ; we set P1 to point to the UART for writing
        xpah    p1
        st 		@-1(p2)
        ldi     uartPortBase & 255                              
        xpal    p1
        st 		@-1(p2)
_GARWait:
        ld      uartStatusWordEnabled(p1)                               ; read the status word
        ani     uartSWEDataAvailable                                    ; wait for data available
        jz      _GARWait
        ld      uartStatusWordEnabled(p1)                               ; re-read it and mask out the error bits.
        ani     uartSWEFramingError+uartSWEParityError+uartSWEOverRunError
        ccl
        adi     0xFF                                                    ; will set carry unless every bit is zero i.e. no errors
        ld      uartReceivedData(p1)                                    ; read the byte in.
        st 		2(p2) 													; save in return space.

        ld 		@1(p2) 													; restore P2.L
        xpal    p2                                                     
        ld 		@1(p2) 													; restore P2.H
        xpah    p2
        ld 		@1(p2) 													; get result.
        xppc    p3                                                      ; and exit.
        jmp     GetART


; *******************************************************************************************************************************
;
;                                            Write byte A to the UART, preserves P1
;
; *******************************************************************************************************************************

PutART: st              @-1(p2)                                                                                                 ; save character to put on stack
        ldi     uartPortBase / 256                                          ; save P1, set it to point to the UART
        xpah    p1                                                      
        st      @-1(p2)
        ldi     uartPortBase & 255
        xpal    p1 
        st      @-1(p2)
_PARWait:
        ld      uartStatusWordEnabled(p1)                               ; wait for the status word to indicate we can send data.
        ani     uartSWETransmitBufferEmpty 
        jz      _PARWait
        ld              @2(p2)                                                  ; get byte to send.
        st      uartDataStrobe(p1)                                      ; send it by writing to the UART

        ld              @1(p2)                                                                                                  ; restore P2.L
        xpal    p2                                                     
        ld              @1(p2)                                                                                                  ; restore P2.H
        xpah    p2
        ld              @1(p2)                                                                                                  ; get result.
        xppc    p3                                                      ; and exit.
        jmp     PutArt
