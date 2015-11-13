; **************************************************************************************************
; **************************************************************************************************
;
;									PDS for Sphere 6800
;
;										 Version:V3N
;
;		Programmed by Eric Jameson 1976 (Sphere Corporation)
;		Transcribed by Paul Robson February 2014 (CRASM format)
;
;		The PDS is a set of programs residing on EPROM for the development of computer programs.
;		It includes a cursor based editor, a mini-assembler and a debugger, and 16 bit utility 
;		routines.
;
;		*IMPORTANT* Crasm incorrectly assembles BLE as $8F not $2F in version 1.7.
;					This will cause a bug in the ASCTOBIN/BINTOASC routines (it is used twice)
;					It assembles to STS so it will crash any emulation.
;
; **************************************************************************************************
; **************************************************************************************************
	
		cpu 6800 										; code for 6800 Microprocessor

		* = $FC00 										; 1k ROM from FC00-FCFF
	
		tmp		= $00
		tmp1	= $02
		arb		= $04 									; 16 bit Pseudo Register B
		ar3 	= $04
		ar2 	= $05
		ara 	= $06 									; 16 bit Pseudo Register A
		ar1 	= $06
		ar0 	= $07
		digit 	= $08									; Digit used for ASCII -> Binary
		outend 	= $0A 									; End of Output Buffer Text
		bufadr 	= $0C 									; Start of I/O Buffer
		bufend 	= $0E 									; End of I/O Buffer
		outbuf 	= $11 									; Start of output buffer
		srcadr 	= $14 									; Source for text moves
		dstadr 	= $16 									; Destination for text moves
		endmem 	= $1A 									; last address of real memory
		csrptr 	= $1C 									; on screen cursor pointer
		bufptr 	= $1E 									; temporary pointer used by outstr
		bufflo 	= $20 									; ptr to end of low edit text
		buffhi 	= $22 									; ptr to start of hi text
		scnptr	= $24 									; ptr to buffered text start
		srcasm 	= $26 									; ptr to assembler source code
		ondval 	= $2A 									; operand value
		symval 	= $2C 									; symbol value
		brksav	= $2E 									; save for breakpoint data
		brkadr 	= $30 									; address of breakpoint
		edit 	= $32 									; 0 if editor not running
		iobuff 	= $35 									; I/O Buffer for debugger
		pcval 	= $40 									; program counter for assembler

		; 80-FF is used by the assembler to store label values.
		
		frstln	= $E01F 								; rightmost character of line 1
		lastln	= $E1E0 								; left most character of bottom line
		lastch	= $E1FF 								; last character on CRT display

		kbdpia 	= $F040 								; Keyboard 2 PIA

; **************************************************************************************************
;
;										System Initialisation
;
;	Sets up initial values upon system reset
;	
; **************************************************************************************************

start	lds 	#$1FF 									; initialise stack pointer
		tsx 											; put in index register
		stx 	srcasm 									; sets assembler output pointer
		stx 	bufadr 									; sets input buffer address
	
		ldaa 	#$1F 									; initialise Keyboard PIA
		staa 	kbdpia+1 								; write 00011111 to Control Register A
														; CA1 Control = 11      (IRQ on CA1 high transition)
														; DDRA Access = 1 		(Peripheral Register, DDR I/P by default)
														; CA2 Control = 011 	(IRQ on CA2 high transition)
														; IRQA1/2 = 0 	  		(Clear Interrupt Flag Bits)

		ldx 	#$FFF 									; location of memory end (assumes 4k machine)
		stx 	bufend 									; end of edit buffer
		stx 	endmem 									; and end of memory

; **************************************************************************************************
;
;										Command Language
;
;	Accepts commands from the keyboard to determine what utility is to be run.	
;
; **************************************************************************************************

exec 	bsr 	home 									; home cursor
		bsr 	clear 									; clear screen

exec1 	bsr 	cr1 									; new line (CR/LF)
		jsr 	inpchr 									; read character from keyboard and echo it
		cmpa 	#$01 									; is it Ctrl-A
		bne 	exec2
		jsr 	asmblr 									; call the assembler

exec2 	cmpa 	#$05 									; is it Ctrl-E
		bne 	exec3
		bsr 	editor 									; call the editor

exec3 	cmpa 	#$12 									; is it Ctrl-R
		bne 	exec4
		bsr 	reedit 									; call re-edit text

exec4 	cmpa	#$04 									; is it Ctrl-D
		bne 	exec1 									; if no, then get another keystroke.
		jmp 	debug 									; yes, go to debugger

; **************************************************************************************************
;
;												Home Cursor
;
; **************************************************************************************************

home	ldx 	#$E000 									; loads X with home position on screen
		stx 	csrptr 									; store in cursor pointer
		rts

; **************************************************************************************************
;
;							  Clear Screen from Cursor Pointer onwards
;
; **************************************************************************************************

clear 	ldab 	#$60 									; load blank character $60
		ldx 	#lastch+1 								; load last charcter on screen, plus one

clear1 	dex 											; go to previous char
		stab 	0,x 									; store blank there
		cpx 	csrptr 									; reached the cursor
		bne 	clear1 									; no keep going
		rts

; **************************************************************************************************
;
;					Input Character into Acc A, blinking cursor while waiting
;
; **************************************************************************************************
		
getchr	ldx 	csrptr 									; invert character at cursor, making it solid
		com 	0,x
		ldx 	#9968 									; delay for blink
get1 	dex
		beq 	getchr 									; when times out, invert character again, flashing

		ldaa 	#$40 		 							; test bit 6 of the keyboard PIA flag register
		bita 	kbdpia+1 								
		beq 	get1 									; if clear, no key available so carry on blink loop

		ldx 	csrptr 									; check to see if the cursor character is solid
		ldaa 	0,x
		bpl 	get2 									; if not, then skip
		com 	0,x 									; re-inverting it
get2 	ldaa 	kbdpia 									; read keyboard character
		rts 

; **************************************************************************************************
;
;	The Editor allows input from the keyboard into a buffer memory. Input is shown on the screen
; 	and can be edited. When the screen is full or editing is finished, it is scrolled into the
; 	edit buffer. It is stored in the top of the edit Buffer from the high buffer pointer (buffhi)
; 	to the end of buffer pointer (bufend)
;
; **************************************************************************************************

editor 	ldx 	bufadr 									; put the buffer address into buff lo
		stx 	bufflo
		ldx 	bufend 									; put buffer end into buffhi
		stx 	buffhi

; **************************************************************************************************
;											Re-enter Editor
; **************************************************************************************************

reedit 	bsr 	home 									; home cursor, clear screen
		bsr 	clear

editrd 	staa 	edit 									; turn editing on.

editin 	ldx  	csrptr 									; set screen pointer to cursor pointer
		stx 	scnptr

edread 	bsr 	getchr 									; X <- csrptr and A <- key pressed
endchr 	cmpa 	#$1B 									; was escape pressed ?
		bne 	ed1 									; skip if not

		clr 	edit 									; clear edit flag
		rts 											; and exit editor.

ed1 	bsr 	insert 									; edits character
		bra 	edread 									; and go round again.

; **************************************************************************************************

cr 		cmpa 	#$0D 									; test for carriage return
		blt 	home 									; if < this then it is home cursor
		bgt 	rtscr 									; if > then next command test

cr1 	ldaa 	#$60 									; load internal return value

; **************************************************************************************************
;									Handle one editor / character
; **************************************************************************************************

insert 	cmpa	#$09 									; test for CTRL+I
		blt  	delete 									; if < this it is a delete command ($08)
		bgt 	cr 										; if > check for return.

insrt1	ldab 	edit 									; check edit flag [[CTRL+I]]
		beq 	insrt2 									; if editor off skip to exit
		jsr 	move2 									; move last line to buffer high
insrt2 	jmp 	scrldn 									; move all lines down 1.
		
rtscr 	cmpa 	#$12 									; check right arrow 
		blt 	sub32									; go to up arrow code.
		bgt 	lftcsr

rtarro 	ldx 	csrptr 									; load cursor pointer [[RIGHT ARROW]]
		bra 	putch1 									; increment cursor and check.

delete 	bsr 	ovr1a 									; scrolls up one line [DELETE]
		bra 	ovr3 									; moves new last screen line.

lftcsr	cmpa 	#$14 									; test if left arrow
		blt 	add32 									; if < go to down arrow code
		bgt 	cler

		dex 											; sub 1 from csrptr [[LEFT ARROW]]
		bra 	add2 									; stores cursor pointer, check underflow

cler 	cmpa 	#$1F 									; test for control back arrow
		blt 	clear 									; if < go to clear screen.
		beq 	lftjst 									; move cursor to left of screen.

; **************************************************************************************************
;			Display character on CRT, Increment Cursor Pointer, check and handle CR
; **************************************************************************************************

putchr	ldx 	csrptr 									; load old cursor pointer
		cmpa 	#$0D 									; is it a CR keystroke ?
		beq 	crlf 									; yes, do a CRLF.

		staa 	0,x 									; write to screen
		cmpa 	#$60 									; is it an internal CR ?
crlf1 	beq 	crlf  									; yes, do the CRLF

putch1 	inx 											; increment cursor pointer
		bra 	add2 									; test for overflow and underflow

sub32 	ldx 	csrptr 									; fetch cursor pointer [[UP ARROW]]
		ldab 	#32 									; we are going to go up one line (e.g. left 32 times)
sub32a 	dex
		decb
		bne 	sub32a 									; so go back 32 positions
		bra 	add2 									; and check for underflow.

add32 	ldab 	#32 									; go forward 32 positions [[DOWN ARROW]]
add32a 	inx
		decb
		bne 	add32a

add2 	stx 	csrptr 									; save cursor pointer

; **************************************************************************************************
;							check for cursor off the top of the screen (NDRFLO)
; **************************************************************************************************

ndrflo 	cpx 	#$E000 									; tests if CSRPTR >= $E000
		bge 	ovrflo 									; go to overflow if okay.

		bsr 	insrt1 									; scroll down and move line.
		bsr 	move3 									; move BUFFLO to the top of the CRT

; **************************************************************************************************
;							check for cursor off the top of the screen (OVRFLO)
; **************************************************************************************************

ovrflo 	cpx 	#$E200 									; test if cursor on screen
		bmi 	ovrext 									; if it is, then exit.
		bsr 	ovr1 									; does OVR1 checking

ovr3 	ldab 	edit 									; check if edit is on.
		beq 	ovrext 									; if off, then exit.

		ldx 	buffhi 									; load hi text pointer
		cpx 	bufend 									; tests if pointers different
		beq 	ovrext 									; exit if no text.
		bsr 	move1a 									; moves chrs to the last line
		ldx 	srcadr 									; resets new buffhi location
		stx 	buffhi

; **************************************************************************************************
;											Force cursor to LHS
; **************************************************************************************************

lftjst 	ldab 	csrptr+1 								; load low byte of ptr
		andb 	#$E0 									; force to left side of this line
		stab 	csrptr+1 								; write it back.	

ovrext 	rts

; **************************************************************************************************
;											  Scroll screen up.
; **************************************************************************************************

ovr1 	ldab 	edit 									; test if edit is on.
		beq 	ovr1a 									; if it is off, just scroll the screen

		ldx 	bufflo 									; load text pointer
		stx 	dstadr 									; destination of text move
		ldx 	scnptr 									; where its coming from.
		bsr		move1 									; move line 1 to buff lo.
		stx 	bufflo 									; save new buffer low pointer

ovr1a 	bra 	scrlup 	

; **************************************************************************************************
;									Carriage Return / Line Feed cursor
; **************************************************************************************************

crlf 	bsr 	add32 									; go down one line.
		bra 	lftjst 									; and carriage return to the left.

; **************************************************************************************************
;		Caclaulates the source address of the data in Bufflo to move to first line on CRT.
; **************************************************************************************************

move3 	ldx 	scnptr 									; csrptr gets $E000
		stx 	dstadr 									; set move address
		ldx 	bufflo 									; loads low buffer adddress
		cpx 	bufadr 									; test if anything there
		beq 	movext 									; exit if empty.
		dex 											; moves back from blank

mv31 	cpx 	bufadr 									; test if srcadr == buffadr
		beq 	mv32 									; moves if start of line
		dex 											; back one character
		ldab 	0,x 									; read it
		cmpb 	#$60 									; is it internal CR
		bne 	mv31 									; no, keep going back.
		inx 											; points back to first char (e.g. after CR)
mv32 	stx 	bufflo 									; savves low address
		bra 	move1

; **************************************************************************************************
;				Move a line from SRCADR to DSTADR (Move1 , from X to DSTADR)
; **************************************************************************************************

move 	ldx 	srcadr 									; load src address into X
move1a 	inx 											; next source character
move1 	ldab 	0,x 									; read source character
		stx 	srcadr 									; save source pointer
		ldx 	dstadr 									; load destination address
		stab 	0,x 									; save it there.
		inx 											; bump the destination address
		stx 	dstadr
		cmpb 	#$60 									; done the complete line
		bne 	move
movext 	rts

; **************************************************************************************************
; 				Move Last line on the screen ot the high area of the buffer (BUFFHI)
; **************************************************************************************************

move2 	ldx 	#lastln 								; address of last line
		clrb

mv21 	pshb
		ldab 	0,x 									; loads source character
		inx
		cmpb 	#$60 									; reached end of line
		bne 	mv21

mv22 	ldx 	buffhi 									; initial destination
mv23 	stab 	0,x 									; stores character (first one $60)
		dex 											; previous location
		stx 	buffhi
		pulb 											; pull off stack
		cmpb 	#0 										; if not terminating zero
		bne 	mv23 									; then keep going round
movex 	rts

; **************************************************************************************************
;									Scroll up and clears last line
; **************************************************************************************************

scrlup 	ldx 	#$E000 									; move the screen data up
scrp1 	ldab 	$20,x
		stab 	0,x
		inx
		cpx 	#lastln
		bne 	scrp1
		stx 	csrptr 									; save as cursor pointer
		stx 	dstadr 									; dest for next move
		jsr 	clear 									; clear last line
		rts

; **************************************************************************************************
;						Scroll all lines down one and clears the top line
; **************************************************************************************************

scrldn 	ldx 	#lastln-1 								; physically move screen data down
scrd1 	ldab 	0,x
		stab 	$20,x
		stx 	csrptr 									; will home cursor at the same time
		dex
		cpx 	#$DFFF
		bne 	scrd1
		ldab 	#$60 									; now clear the top line
scrd2 	inx
		stab	0,x
		cpx 	#frstln
		bne 	scrd2
		rts

; **************************************************************************************************
;							Output String between OutBuf and BufEnd
; **************************************************************************************************

outstr 	ldx 	outbuf 									; start of text
out1 	ldaa 	0,x 									; output character
		stx 	bufptr 									; save position
		jsr 	putchr 									; print character
		ldx 	bufptr 									; restore position
		cpx 	outend 									; exit if at end
		beq 	out2
		inx 											; else bump pointer and do next.
		bra 	out1
out2 	rts

; **************************************************************************************************
;
;			Mini assembler - A fixe field one instruction per line 2 pass assembler
;
; **************************************************************************************************


asmblr 	clr 	ar3 									; pass counter to zero (first pass)

asm1 	ldx  	bufflo 									; set PC counter to start of object code (end of edit)
		stx 	pcval

		ldx 	srcasm 									; loads address of first line.
; **************************************************************************************************
;				Next Instruction. First Part processes the label and possible equate
; **************************************************************************************************

asm1a 	stx 	tmp1 									; save line addresss
		ldaa 	8,x 									; loads symbol (label)
		ldab 	7,x 									; load operand type code (R,D,E or blank)
		cmpb 	#'@'									; if it is @, loads data in symbol
		beq 	indadr 								
		inx 											; set index to start of operand number (add 7 to x)
		inx
		inx
		inx
		inx
		inx
		inx
		jsr 	ascbin 									; convert number to binary in B:A
asm1b 	stab 	ondval 									; store operand value in ondval
		staa 	ondval+1

symbl	ldx 	tmp1 									; loads line pointer back into index register
		ldaa 	0,x 									; loads the label for this instruction into A
		ldab 	1,x 									; load label control character into B
		ldx 	pcval 									; copy PCVal to SYMVal (e.g. this is going to be equated)

		stx 	symval
		cmpb 	#'=' 									; is it an equate
		bne 	asm2  									; if not equated skip

		ldx 	ondval 									; because it is a= <xxx> then use the operand value as the
		stx 	symval 									; symbol value
		bra 	asm3  									; and continue the assembly.

asm2 	cmpb 	#" "									; test for end of program - something other than space.
		beq 	asm3 									; if it is a space then assemble it.

		tst 	ar3 									; is it the second pass ? (e.g. ar3 is non zero)
		beq 	asm2a 						 			; if it is then exit
		rts
asm2a 	stab 	ar3 									; set ar3 to non-zero to mark pass 2
		bra 	asm1 									; and run a second pass.

asm3 	bsr 	symptr 									; set X for symble table address of A
		ldaa 	symval 									; and copy symval into it
		staa 	0,x 									; note space/blank is used for non-label lines, so this is always done.
		ldaa 	symval+1
		staa 	1,x

; **************************************************************************************************
;									Construct the operation code.
; **************************************************************************************************

ldop 	ldx 	tmp1 									; get the line start
		inx 											; and point the opcode character.
		inx
		inx
		ldaa 	0,x 									; look at the opcode character
		cmpa 	#' ' 									; is there an opcode there
		beq 	oprnd 									; if no, then it is a data item.
		jsr 	ascbin 									; evaluate the operand
		ldx 	pcval 									; stores into object code
		staa 	0,x
		inx 											; bump object code pointer and save back
		stx 	pcval

; **************************************************************************************************
;										Construct the operand
; **************************************************************************************************

oprnd 	ldx 	tmp1									; get current line
		ldaa 	6,x 									; get operansd size char (R,E,D)
		ldx 	pcval 									; object code pointer.
		cmpa 	#'E' 									; compare against E
		bgt 	reltiv 									; if > then it is R, do relative code
		beq 	extend 									; if = then it is extended mode.
		cmpa 	#"D" 									; if it is D then do direct
		beq 	direct

asm4 	ldx 	tmp1 									; go to next line
asm4a 	inx 											; go down line looking for end of line marker
		ldaa 	0,x
		cmpa 	#$60
		bne 	asm4a
		inx 											; skip EOL marker
		bra 	asm1a 									; and do the next line.

indadr 	bsr 	symptr 									; point X to the symbol (operand was @something)
		ldx 	0,x 									; read the value stored there
		stx 	ondval 									; use that as an operand
		bra 	symbl 									; continue assembly from the 'decode label' point

; **************************************************************************************************
;								Convert Symbol to Address of Symbol Table
; **************************************************************************************************

symptr 	asla 											; B:A is symbol x 2
		clrb
loadx	staa 	tmp+1 									; and copy that into X
		stab 	tmp
		ldx 	tmp
		rts

extend 	ldab 	ondval 									; process extended, direct operands
		stab 	0,x
		inx
direct 	ldaa 	ondval+1
		staa	0,x
		inx
		stx 	pcval 									; write pointer back
		bra 	asm4 									; go to next line

reltiv 	inx 											; increment PC Pt to point to next byte and save it.
		stx 	pcval
		ldaa 	ondval+1 								; load low byte of operand
		suba 	pcval+1 								; calculate relative offset of byte after the instruction
		dex 											; write it in the place just skipped past
		staa 	0,x
		bra 	asm4 									; go to next line.

; **************************************************************************************************
;
;												Debugger
;
; **************************************************************************************************

bkentr 	tsx 											; stack pointer to index (BRK comes here)
		ldab 	5,x 									; get return address (hi)
		ldaa 	6,x 									; (lo)
		suba 	#1 										; decrement return address
		sbcb	#0 										; (16 bit value)
		stab 	5,x 									; make the return address point to the breakpoint
		staa 	6,x 									
		bra 	debug 									; and enter the debugger.

line 	cmpa 	#13 									; test for CR
		blt 	jmplcn 									; if < goto 'JSR' (Ctrl+J) routine
		bgt 	opnreg 									; if > goto next (Ctrl+R) tests
		jsr 	sub32 									; scroll up one line.
poplin 	ins 											; tidy up stack for display of CR
		ins

debug 	bsr 	newlin  								; print CR then > (debug prompt)
dbug1 	bsr 	inpchr 									; reset command
		jsr 	rtarro 									; move cursor right
		ldx 	pcval 									; load currently opened location
		bsr 	runbug 									; execute debug command
		bra 	dbug1 									; go back to next command.

inpchr 	jsr 	getchr 									; read a character and echo it.
		jsr 	putchr 
		rts

runbug 	cmpa 	#03 									; check for Control+C
		blt 	brkset 									; if < it is Ctrl+B, set breakpoint
		bgt 	exit 								 	; if > skip to next command text

clrbrk 	ldx 	brkadr 									; [[clear break point]]
		ldaa 	brksav 									; save inserted breakpoint at (brkadr)
		staa 	0,x
		bra 	dspadr 									; go to open the location

change 	cmpa 	#" " 									; check for a space command.
		blt 	exectv 									; if < return to executive (Ctrl+X)
		bgt 	opnpre 									; if > other command tests

space 	bsr 	inpnum 									; Space [[Change Memory]]	
		ldx 	pcval 									; write inputted number at pcval address
		staa 	0,x 	

opnnxt 	inx 											; go to next location address [[next address]]
		bra 	dspadr 									; go to open location.

opnpre 	cmpa 	#'-' 									; is it -
		blt 	opnnxt 									; if <, assume + and move to the next address
		dex 												; otherwise go to [[previous address]]
		bra 	dspadr 									; and open the location

exectv 	ins 											; clean up the stack (remove the call) [[go executive]]
		ins
		jmp 	exec 									; return to the executive.

brkset 	ldaa 	0,x 									; set breakpoint [[set break]]
		staa 	brksav 									; save byte which will be replaced by SWI
		stx 	brkadr 									; save breakpoint address
		ldaa 	#$3F 									; $3F is 6800 SWI Instruction
		staa 	0,x 									; overwrite code
		bra 	poplin 									; and fix up stack and do next instruction.

opnreg 	cmpa 	#$12 									; tests for (Ctrl+R) TOS
		blt 	opnloc 									; if < go to openlocation
		bgt 	opntbl 									; if > next test

		tsx 											; access TOS and lose 2 bytes used to call this routine [[display stack]]
		inx
		inx
		bra 	dspadr 									; go to display routine.

setstk 	txs 											; set stack - stack pointer gets PCVAL [[set stack pointer]]
		bra 	debug 									; go to input command.

opnloc 	bsr 	inpnum 									; load a 16 bit number [[go to a location]]
opnlc1 	stab 	pcval 									; save in PCVAL
		staa 	pcval+1 
		bra 	dspad1 									; display that location

exit 	cmpa 	#$07 									; is it conrol E
		beq 	golocn 									; if so, go to the go command
		bgt 	line 									; go to next command test
		ins 											; clear up the stack and return
		ins
		rti 

opntbl 	cmpa 	#$14 									; test if Ctrl+T (table)
		blt 	setstk 									; if < goto set stack
		bgt 	change 									; if > change memory location
		bsr 	inpchr 									; load a with a symbol from the keyboard
		asla 											; set AB to the location of that symbols value
		clrb
		bra 	opnlc1 									; save and display address.

golocn 	ins 											; go to location - clean up the stack
		ins		
jmplcn 	jmp 	0,x 									; jump to users program

newlin 	ldaa 	#$0D 									; print CR
		bsr 	pntbf1
		ldaa	#">" 									; print > prompt
		bra 	pntbf1 

inpnum 	jsr 	editin 									; input a string of digits
		ldx 	scnptr 									; load address of first digit
		bsr 	ascbin 									; convert to integer
		rts 											; return to caller.

dspadr 	stx 	pcval 									; save pc value
dspad1 	bsr 	newlin 									; print CR >
		bsr 	pntdig 									; print out PC value in hexadecimal.
		jsr 	rtarro 									; move right
		ldx 	pcval 									; read byte at pcval
		ldaa 	0,x
		bsr 	pntbyt 									; print it and return
		rts

pntdig 	ldaa 	pcval 									; print pcval
		bsr 	pntbyt
		ldaa 	pcval+1
pntbyt	ldx 	#16 									; base to convert
		stx 	arb
		clrb
		ldx 	#iobuff 								; address of IO Buffer

convrt 	stab 	iobuff+1 								; clear byte for second digit
		jsr 	binasc 									; convert to ASCII digits
		ldaa	iobuff 									; load high digit
		ldab 	iobuff+1 								; test both digits converted
		bne 	pntbuf 									; skips if both digits converted
		staa 	iobuff+1 								; set up low digit
		ldaa 	#"0" 							
pntbuf 	bsr 	pntbf1 									; print high digit
		ldaa 	iobuff+1 								; get low digit
pntbf1 	jsr 	putchr 									; displays character
		rts

; **************************************************************************************************
;
;		ASCII to Binary Conversion. String pointed to by X, Result in BA. Base is in ARA
;
; **************************************************************************************************

ascbin  ldaa	0,x 									; look at first character
		cmpa 	#'.' 									; is it a decimal point (=> base 10)
		blt 	oct 									; if < assume base 8 (*xxxx)
		bgt 	hex 									; if > assume base 16
		ldaa 	#10 									; if = it is decimal
		bra 	asc1
oct 	ldaa 	#8 										; it is octal
asc1	inx 											; skip over . or *
		bra 	asc2 									; and continue
hex 	ldaa 	#16 									; base 16
asc2 	staa 	ar0 									; store base in AR0

entr2 	clrb 											; Number is kept in B,TOS
		pshb
		stab 	ar1 									; clear high of base

nxtchr 	ldaa 	0,x 									; read next char
		inx
		cmpa 	#'0'									; if < 0 then exit
		blt 	aexit
		suba 	#'0' 									; subtract ascii zero.
		cmpa 	#10 									; if in range 0-9 then continue
		blt 	asc3
		cmpa 	#16 									; if in range 10-16 not a hex digit
		ble 	aexit
		suba 	#7 										; now it is in the range 0-15 or more 
		cmpa 	#16 									; out of range (e.g. > 'F' originally)
		bge 	aexit 									; then exit

asc3 	staa 	digit 									; save the now valid hex digit
		stx 	tmp 									; save X register
		pula 											; B:A now contains number so far.
		bsr 	mult 									; multiply this by the base stored in AR0/1
		adda 	digit 									; add the digit
		adcb 	#0 										; add carry into MSB
		psha											; push back on the stack
		ldx 	tmp 									; restore string pointer
		bra 	nxtchr 									; do the next character
aexit 	pula 											; restore A off stack, number now in B:A
		rts

; **************************************************************************************************
;
;				Binary to ASCII - Number in BA, Base is in ARB, X points to output 
;
; **************************************************************************************************

binasc 	stx 	tmp 									; save output pointer
		des 											; sets TOS to $FF identifying end of string on stack
		tsx
		clr 	0,x
		com 	0,x

bin1 	ldx 	arb 									; copy ARB to ARA
		stx 	ara
		bsr 	divide 									; divide B:A by ARB

		staa 	tmp1 									; save A in TMP1
		ldaa 	ar0 									; load digit (remainder) from ar0
		psha 											; save on stack
		ldaa 	tmp1 									; restore B:A as quotient

		tsta 											; if quotient is nonzero go back and divide again
		bne 	bin1
		tstb
		bne 	bin1

binstr 	ldx 	tmp 									; reload output pointer, destack the result.
bin3 	pula 											; restore a digit
		tsta 											; if negative then reached the end of the string
		bpl 	bin4 
		rts 											; so exit.
bin4 	cmpa 	#9 										; tests if result is hex
		ble 	bin5 	
		adda 	#7 										; if it is A-F add 7 to shift chars to A-F
bin5 	adda 	#'0'									; make an ASCII character
		staa	0,x 									; save at output
		inx 											; bump to next character
		bra 	bin3 									; and destack another character.

; **************************************************************************************************
;
;									16 Bit Multiply : BA = BA * ARA
;
; **************************************************************************************************

mult 	psha 											; push the value onto the stack
		pshb 	
		ldaa 	#16 									; number of shifts
		psha 											; also on the stack
		clra 											; zero result
		clrb 
		tsx 											; point index to stack

mul1 	asla 											; shift BA left
		rolb
		asl 	2,x 									; shift original BA MSB into carry
		rol 	1,x
		bcc 	mul2 									; if MSB was 1.
		adda 	ar0 									; add ara into ba, it will be shifted into position.
		adcb 	ar1 	
mul2 	dec 	0,x 									; decrement counter
		bne 	mul1 									; do this 16 times to multiply.
		ins 											; fix stack back up, result is in BA already.
		ins
		ins
		rts 

; **************************************************************************************************
;
;					16 Bit Divide : BA = BA / ARA, on exit ARA is the remainder
;
; **************************************************************************************************

divide 	psha 											; save dividend into X3,4 (eventually)
		pshb
		ldaa 	ar1 									; puts divisor into X1,2 (eventually)
		ldab 	ar0
		pshb 
		psha
		des 											; space for count byte
		tsx 											; index points to stack.
		ldaa 	#1 										; the count
		tst 	1,x 									; if hi divisor bit on 
		bmi 	div2 									; then only do it once
div1 	inca 											; otherwise, add 1 for every leading zero and shift the divisor
		asl 	2,x 									; shift the divisor
		rol 	1,x
		bmi 	div2 									; until divisor is left justified
		cmpa 	#17 									; if done 17 times divisor was zero
		bne 	div1 									; so fall through.
div2 	staa 	0,x 									; save counter.

		ldab 	3,x 									; BA is original dividend value
		ldaa 	4,x
		clr 	3,x 									; clear quotient
		clr 	4,x

div3 	suba 	2,x 									; divide loop - subtract LJ divisor from dividend
		sbcb 	1,x
		bcc 	div4 									; skip if dividend < divisor
		adda 	2,x 									; fix divisor back
		adcb 	1,x
		clc 											; clear carry to shift into result.
		bra 	div5
div4 	sec 											; dividend >= divisor so keep subtraction and shift 1 into result

div5 	rol 	4,x 									; shift carry into result
		rol 	3,x 	
		lsr 	1,x 									; shift dividend right 1
		ror 	2,x
		dec 	0,x 									; do it count times
		bne 	div3

		stab 	ar1 									; save remainder in ARA
		staa 	ar0
		ins 											; remove count and divisor from stack
		ins
		ins
		pulb  											; restore quotient into BA
		pula 	
		rts 											; and end division

; **************************************************************************************************
;
;												6800 Vectors
;
; **************************************************************************************************

		* = 0xFFF8 										; always goes at $FFF8
		dw 		0x104 									; IRQ vector
		dw 		bkentr 									; SWI vector
		dw 		0x108 									; NMI vector
		dw 		start 									; RST vector

