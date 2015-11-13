; *****************************************************************************************************
; *****************************************************************************************************
;
;							OSI-65A Source Code (formatted for XA65)
;							========================================
;
;	Converted by Paul Robson (paul@robsons.org.uk)
;
; *****************************************************************************************************
; *****************************************************************************************************
		
CurrentAddress = $FC 										; Working address
CurrentData = $FF 											; Data to be stored at working address
															; (note $FE is not actually used)

CPU_Y = $129												; Registers that are copied into the 6502 when Go is executed
CPU_X = $12A
CPU_A = $12B
CPU_Status = $12C
CPU_SP = $12D
CPU_PCTR_High = $12E
CPU_PCTR_Low = $12F

NMI_Routine = $130											; 6502 Interrupt Vectors.
IRQ_Routine = $1C0

ACIA_Control = $FC00										; 6850 ACIA Control (Write)
ACIA_Status = $FC00											; 6850 ACIA Status (Read)
ACIA_Data = $FC01											; 6850 ACIA Data Registers

StackLow = $26												; Stack starts at $126.

		.org	$FE00                   					; Note, ROM multiply imaged at $FE00 and $FF00

; *****************************************************************************************************
;
;		Read ASCII Character into A from ACIA and Echo it 
;
; *****************************************************************************************************

InputCharacter                  			
		lda	ACIA_Status                 					; Put Receive Data Register Full (Bit 0) of ACIA Status into C
		lsr	a                  
		bcc	InputCharacter                  				; Wait for the Input Character to be ready
		lda	ACIA_Data                						; Read the character from the TTY Unit
		and	#$7F                  							; Drop the parity bit, now standard ASCII.

; *****************************************************************************************************
;
;		Print Character in A via ACIA
;
; *****************************************************************************************************

OutputCharacter                  
		pha                  								; Save so A is preserved by this routine, also for Input Character.
WaitTDREFlag             
		lda	ACIA_Status                  					; Put Transmit Data Register Empty (Bit 1) of ACIA Status into
		lsr	a                  	
		lsr	a                  	
		bcc	WaitTDREFlag                  					; Wait until the TDR is clear so we can send the character
		pla                  								; restore A
		sta	ACIA_Data                  						; and write to the TDR
		rts                  								; and return
				
InputHexDigit                  
		jsr	InputCharacter                  				; Read a character.
		cmp	#$52                  							; if it is an "R", restart the monitor
		beq	ProcessorReset                  
		cmp	#$30                  							; must be in range 0-9, A-F
		bmi	InputHexDigit         							; is < "0"         							
		cmp	#$3A                  
		bmi	ExitInputHexDigit                  				; if < "9"+1, exit with lower 4 bits
		cmp	#$41                  							; fail if < "A"
		bmi	InputHexDigit                  					; fail if >= "G"
		cmp	#$47                  
		bpl	InputHexDigit                  
		clc                  								; adjusts it so the lower four bits are correct e.g. $41 => $3A
		sbc	#$06                  							; (sec ; sbc #$07 is clearer !)
ExitInputHexDigit                  	
		and	#$0F                  							; make a valid four digit value
		rts                  								; and return

; *****************************************************************************************************
;
;		Processor Reset comes here - Resets the ACIA and get the first command.
;
; *****************************************************************************************************
				
ProcessorReset                  
		lda	#$03        									; Master reset ACIA, write 0 to CR0 and CR1          
		sta	ACIA_Control                  
		lda	#$B1          									; Write %1 01 100 01 to ACIA Control        
		sta	ACIA_Control									; Divide master clock by 16. 8 Data bits and 2 stop bits.
															; Transmitting Interrupt Enabled. Receive Interrupt Enabled.                

		cld                  								; Make sure we are in Binary Mode.

; *****************************************************************************************************
;
;		Get the next command.
;
; *****************************************************************************************************

MonitorStart                  
		sei                  								; Disable Interrupts
		ldx	#StackLow                  						; Set up the Stack 
		txs                  
		lda	#$0D                  							; Print Carriage Return and Line Feed
		jsr	OutputCharacter                  
		lda	#$0A                  		
		jsr	OutputCharacter                  
		jsr	InputCharacter              					; Read character from Input Device.    
		cmp	#$4C											; (L)oad Command                  
		beq	LoadProgram                  
		cmp	#$50                  							; (P)rint Command
		beq	PrintMemory                  
		cmp	#$47                  							; (G)o Command
		bne	ProcessorReset                  

; *****************************************************************************************************
;
;	(G)o command. Read the Registers from the end of page 0 into memory, then execute it using
;	an RTI to load PCH/PCL/Status.
;
; *****************************************************************************************************

		ldx	CPU_SP                  						; Execute the go command - load Registers from memory.
		txs                  								; Load Stack Pointer
		ldx	CPU_X                  							; Load X and Y register
		ldy	CPU_Y                  
		lda	CPU_PCTR_High                  					; Push PCTR, High first
		pha                  
		lda	CPU_PCTR_Low                  
		pha                  
		lda	CPU_Status                  					; Push status
		pha                  
		lda	CPU_A                  							; Get accumulator
		rti                  								; and start from wherever.

; *****************************************************************************************************
;
;		(L)oad command. Reads 4 digits in as the start address, then reads data into consecutive
;		bytes using each subsequent 2 hex digits.
;		Exits using the "R" code in the Input Digit Routine.
;				
; *****************************************************************************************************

LoadProgram                  
		jsr	BuildAddress     								; Read 4 hex digits to make an address             
		ldx	#CurrentData-CurrentAddress						; Read into Current Data
		ldy	#$00       										; Clear Y.           
LoadAddressLoop                  
		jsr	ReadByteIntoMemory                  			; Read 2 hex digits into Current Data
		lda	CurrentData                  					; Read the byte that was read
		sta	(CurrentAddress),y                  			; Store at the current location.
		iny                  								; Go to next byte
		bne	LoadAddressLoop                  
		inc CurrentAddress+1                  				; If Y overflows increment the MSB of the Current Address
		clv                  								; ... and go round again ... why not BRA ?
		bvc	LoadAddressLoop

; *****************************************************************************************************
;		
;		(P)rint Memory. Gets 4 keystrokes which specify the start address.
;		Prints 8 bytes per line with a space after each.
;		Terminated by typing any key on the TTY.
;
; *****************************************************************************************************

PrintMemory                  
		jsr	BuildAddress                  					; Read 4 digits to make an address
		ldy	#$00                  							; Clear Y.
PrintLineLoop                  
		ldx	#$09                  
		lda	#$0D                  							; Print a carriage return.
		jsr	OutputCharacter                  
		lda	#$0A                  
		jsr	OutputCharacter                  
PrintDataLoop                  
		dex                  								; Done 8 of them
		beq	PrintExitCheck                  				; if so, check if a key has been pressed, if not loop back above. 
		jsr	PrintCurrentAddressData                  		; Print byte at (current address),Y
		iny                  								; Go to next byte
		bne	PrintDataLoop                  
		inc	CurrentAddress+1                  				; Adjust MSB if overflowed LSB
		jmp	PrintDataLoop                  					; and keep printing data
				
PrintExitCheck                  
		lda	ACIA_Status        								; Check Bit 0 of Status (Receive Data Register Full)          
		lsr	a                  
		bcs	MonitorStart                  					; If it is set, then a key has been pressed, exit printing.
		nop                  
		bcc	PrintLineLoop                  					; otherwise go do another line.

; *****************************************************************************************************
;
;		Read a byte into memory from the keyboard at CurrentAddress+X
;
; *****************************************************************************************************

ReadByteIntoMemory                  
		jsr	InputHexDigit      								; Read the upper hexadecimal digit.            
		asl	a                  								; move it into bits 4-7.
		asl	a                  
		asl	a                  
		asl	a                  
		sta	CurrentAddress,x                  				; save it at the required address
		jsr	InputHexDigit                  					; Read the lower hex digit
		clc            										; Make it into a byte      
		adc	CurrentAddress,x                  				; (could have done OR here)
		sta	CurrentAddress,x                  				; and update it
		rts                  								; and return.

; *****************************************************************************************************
;
;		Read a 4 digit hexadecimal world into CurrentAddress, CurrentAddress+1
;
; *****************************************************************************************************
				
BuildAddress                  
		ldx	#$01                  							; this makes the effective address read CurrentAddress+1 e.g. high byte
		jsr	ReadByteIntoMemory                  			; Read Address High Byte in.
		dex                  								; now point to CurrentAddress
		jsr	ReadByteIntoMemory                  			; Read Address Low Byte in.
		rts                  
				
PrintHexDigit                  
		clc                  								; make it $30-$3F
		adc	#$30                  
		cmp	#$3A                  							; is it $3A+ (e.g. A-F)
		bcs	PHDIsAToF                  						; if so, adjust it, if 0-9 just print it.
PHDOutput                  
		jsr	OutputCharacter                  				; print the character out.
		rts                  
				
PHDIsAToF                  									; it is A-F
		adc	#$06                  							; correct the ASCII
		bcc	PHDOutput                  						; and print it.

; *****************************************************************************************************
;
;		Print Byte at Current Address followed by Space.
;
; *****************************************************************************************************

PrintCurrentAddressData                  
		lda	(CurrentAddress),y 								; Read current byte                  
		and	#$F0                  							; Get the upper 4 bits
		lsr	a                  
		lsr	a                  
		lsr	a                  
		lsr	a                  
		jsr	PrintHexDigit                  					; and Print them.
		lda	(CurrentAddress),y                  			; Read the current byte again.
		and	#$0F                  							; Get the lower 4 bits
		jsr	PrintHexDigit                  					; and print them.
		lda	#$20                  							; print a space.
		jsr	OutputCharacter                  
		rts                  

		.byte 		$40										; junk
		.byte 		$9d		

		.word		NMI_Routine								; 6502 Vectors. The ROM is at $FExx and $FFxx
		.word		ProcessorReset							; so these function as the standard 6502
		.word		IRQ_Routine								; Reset and Interrupt vectors.
