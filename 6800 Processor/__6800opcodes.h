case 0x01: /* $01 nop */
	CYCLES(2);{};break;
case 0x06: /* $06 tap */
	CYCLES(2);putCCR(A);break;
case 0x07: /* $07 tpa */
	CYCLES(2);A = getCCR();break;
case 0x08: /* $08 inx */
	CYCLES(3);IX++;zValue = IX | (IX >> 8);break;
case 0x09: /* $09 dex */
	CYCLES(3);IX--;zValue = IX | (IX >> 8);break;
case 0x0a: /* $0a clv */
	CYCLES(2);overflowFlag = 0;break;
case 0x0b: /* $0b sev */
	CYCLES(2);overflowFlag = 1;break;
case 0x0c: /* $0c clc */
	CYCLES(2);carryFlag = 0;break;
case 0x0d: /* $0d sec */
	CYCLES(2);carryFlag = 1;break;
case 0x0e: /* $0e cli */
	CYCLES(2);interruptMask = 0;break;
case 0x0f: /* $0f sei */
	CYCLES(2);interruptMask = 1;break;
case 0x10: /* $10 sba */
	CYCLES(2);carryFlag = 0;A = _CPUSub8Bit(A,B);break;
case 0x11: /* $11 cba */
	CYCLES(2);carryFlag = 0;_CPUSub8Bit(A,B);break;
case 0x16: /* $16 tab */
	CYCLES(2);nValue = zValue = B = A;overflowFlag = 0;break;
case 0x17: /* $17 tba */
	CYCLES(2);nValue = zValue = A = B;overflowFlag = 0;break;
case 0x19: /* $19 daa */
	CYCLES(2);_CPUDecimalAdjust();break;
case 0x1b: /* $1b aba */
	CYCLES(2);carryFlag = 0;A = _CPUAdd8Bit(A,B);break;
case 0x20: /* $20 bra @1 */
	CYCLES(3);BRANCHIF(-1);break;
case 0x22: /* $22 bhi @1 */
	CYCLES(3);BRANCHIF(carryFlag == 0 && zValue != 0);break;
case 0x23: /* $23 bls @1 */
	CYCLES(3);BRANCHIF(!(carryFlag == 0 && zValue != 0));break;
case 0x24: /* $24 bcc @1 */
	CYCLES(3);BRANCHIF(carryFlag == 0);break;
case 0x25: /* $25 bcs @1 */
	CYCLES(3);BRANCHIF(carryFlag != 0);break;
case 0x26: /* $26 bne @1 */
	CYCLES(3);BRANCHIF(zValue != 0);break;
case 0x27: /* $27 beq @1 */
	CYCLES(3);BRANCHIF(zValue == 0);break;
case 0x28: /* $28 bvc @1 */
	CYCLES(3);BRANCHIF(overflowFlag == 0);break;
case 0x29: /* $29 bvs @1 */
	CYCLES(3);BRANCHIF(overflowFlag != 0);break;
case 0x2a: /* $2a bpl @1 */
	CYCLES(3);BRANCHIF((nValue & 0x80) == 0);break;
case 0x2b: /* $2b bmi @1 */
	CYCLES(3);BRANCHIF((nValue & 0x80) != 0);break;
case 0x2c: /* $2c bge @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) == overflow);break;
case 0x2d: /* $2d blt @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) != overflow);break;
case 0x2e: /* $2e bgt @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) == overflowFlag && zValue != 0);break;
case 0x2f: /* $2f ble @1 */
	CYCLES(3);BRANCHIF(!((nValue >> 7) == overflowFlag && zValue != 0));break;
case 0x30: /* $30 tsx */
	CYCLES(3);IX = SP + 1;break;
case 0x31: /* $31 ins */
	CYCLES(3);SP++;break;
case 0x32: /* $32 pula */
	CYCLES(4);PULLBYTE();A = MB;break;
case 0x33: /* $33 pulb */
	CYCLES(4);PULLBYTE();B = MB;break;
case 0x34: /* $34 des */
	CYCLES(3);SP--;break;
case 0x35: /* $35 txs */
	CYCLES(3);SP = IX - 1;break;
case 0x36: /* $36 psha */
	CYCLES(3);MB = A;PUSHBYTE();break;
case 0x37: /* $37 pshb */
	CYCLES(3);MB = B;PUSHBYTE();break;
case 0x39: /* $39 rts */
	CYCLES(5);PULLWORD();PC = temp16;break;
case 0x3b: /* $3b rti */
	CYCLES(10);PULLBYTE();putCCR(MB);PULLBYTE();B=MB;PULLBYTE();A=MB;PULLWORD();IX = temp16;PULLWORD();PC = temp16;break;
case 0x3f: /* $3f swi */
	CYCLES(12);_CPUSoftwareInterrupt( );break;
case 0x40: /* $40 nega */
	CYCLES(2);carryFlag = 0;A = _CPUSub8Bit(0,A);break;
case 0x43: /* $43 coma */
	CYCLES(2);carryFlag = 1;overflowFlag = 0;nValue = zValue = A = A ^ 0xFF;break;
case 0x44: /* $44 lsra */
	CYCLES(2);A = _CPULSRShift(A);break;
case 0x46: /* $46 rora */
	CYCLES(2);A = _CPURORRotate(A);break;
case 0x47: /* $47 asra */
	CYCLES(2);A = _CPUASRShift(A);break;
case 0x48: /* $48 asla */
	CYCLES(2);A = _CPUASLShift(A);break;
case 0x49: /* $49 rola */
	CYCLES(2);A = _CPUROLRotate(A);break;
case 0x4a: /* $4a deca */
	CYCLES(2);overflowFlag = (A == 0x80) ? 1 : 0;nValue = zValue = --A;break;
case 0x4c: /* $4c inca */
	CYCLES(2);overflowFlag = (A == 0x7F) ? 1 : 0;nValue = zValue = ++A;break;
case 0x4d: /* $4d tsta */
	CYCLES(2);carryFlag = overflowFlag = 0;zValue = nValue = A;break;
case 0x4f: /* $4f clra */
	CYCLES(2);A = nValue = zValue = overflowFlag = carryFlag = 0;break;
case 0x50: /* $50 negb */
	CYCLES(2);carryFlag = 0;B = _CPUSub8Bit(0,B);break;
case 0x53: /* $53 comb */
	CYCLES(2);carryFlag = 1;overflowFlag = 0;nValue = zValue = B = B ^ 0xFF;break;
case 0x54: /* $54 lsrb */
	CYCLES(2);B = _CPULSRShift(B);break;
case 0x56: /* $56 rorb */
	CYCLES(2);B = _CPURORRotate(B);break;
case 0x57: /* $57 asrb */
	CYCLES(2);B = _CPUASRShift(B);break;
case 0x58: /* $58 aslb */
	CYCLES(2);B = _CPUASLShift(B);break;
case 0x59: /* $59 rolb */
	CYCLES(2);B = _CPUROLRotate(B);break;
case 0x5a: /* $5a decb */
	CYCLES(2);overflowFlag = (A == 0x80) ? 1 : 0;nValue = zValue = --B;break;
case 0x5c: /* $5c incb */
	CYCLES(2);overflowFlag = (A == 0x7F) ? 1 : 0;nValue = zValue = ++B;break;
case 0x5d: /* $5d tstb */
	CYCLES(2);carryFlag = overflowFlag = 0;zValue = nValue = B;break;
case 0x5f: /* $5f clrb */
	CYCLES(2);B = nValue = zValue = overflowFlag = carryFlag = 0;break;
case 0x60: /* $60 neg @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();carryFlag = 0;MB = _CPUSub8Bit(0,MB);WRITE();break;
case 0x63: /* $63 com @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();carryFlag = 1;overflowFlag = 0;nValue = zValue = MB ^ 0xFF;WRITE();break;
case 0x64: /* $64 lsr @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();MB = _CPULSRShift(MB);WRITE();break;
case 0x66: /* $66 ror @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();MB = _CPURORRotate(MB);WRITE();break;
case 0x67: /* $67 asr @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();MB = _CPUASRShift(MB);WRITE();break;
case 0x68: /* $68 asl @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();MB = _CPUASLShift(MB);WRITE();break;
case 0x69: /* $69 rol @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();MB = _CPUROLRotate(MB);WRITE();break;
case 0x6a: /* $6a dec @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();overflowFlag = (MB == 0x80) ? 1 : 0;nValue = zValue = --MB;WRITE();break;
case 0x6c: /* $6c inc @1,x */
	CYCLES(6);FETCH();MA = MB + IX;overflowFlag = (MB == 0x7F) ? 1 : 0;nValue = zValue = ++MB;WRITE();break;
case 0x6d: /* $6d tst @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READ();carryFlag = overflowFlag = 0;zValue = nValue = MB;break;
case 0x6e: /* $6e jmp @1,x */
	CYCLES(3);FETCH();MA = MB + IX;PC = MA;break;
case 0x6f: /* $6f clr @1,x */
	CYCLES(6);FETCH();MA = MB + IX;nValue = zValue = overflowFlag = carryFlag = 0;MB = 0;WRITE();break;
case 0x70: /* $70 neg @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();carryFlag = 0;MB = _CPUSub8Bit(0,MB);WRITE();break;
case 0x73: /* $73 com @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();carryFlag = 1;overflowFlag = 0;nValue = zValue = MB ^ 0xFF;WRITE();break;
case 0x74: /* $74 lsr @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();MB = _CPULSRShift(MB);WRITE();break;
case 0x76: /* $76 ror @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();MB = _CPURORRotate(MB);WRITE();break;
case 0x77: /* $77 asr @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();MB = _CPUASRShift(MB);WRITE();break;
case 0x78: /* $78 asl @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();MB = _CPUASLShift(MB);WRITE();break;
case 0x79: /* $79 rol @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();MB = _CPUROLRotate(MB);WRITE();break;
case 0x7a: /* $7a dec @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();overflowFlag = (MB == 0x80) ? 1 : 0;nValue = zValue = --MB;WRITE();break;
case 0x7c: /* $7c inc @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;overflowFlag = (MB == 0x7F) ? 1 : 0;nValue = zValue = ++MB;WRITE();break;
case 0x7d: /* $7d tst @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READ();carryFlag = overflowFlag = 0;zValue = nValue = MB;break;
case 0x7e: /* $7e jmp @1@2 */
	CYCLES(3);FETCHWORD();MA = temp16;PC = MA;break;
case 0x7f: /* $7f clr @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;nValue = zValue = overflowFlag = carryFlag = 0;MB = 0;WRITE();break;
case 0x80: /* $80 suba #@1 */
	CYCLES(2);FETCH();carryFlag = 0;A = _CPUSub8Bit(A,MB);break;
case 0x81: /* $81 cmpa #@1 */
	CYCLES(2);FETCH();carryFlag = 0;_CPUSub8Bit(A,MB);break;
case 0x82: /* $82 sbca #@1 */
	CYCLES(2);FETCH();A = _CPUSub8Bit(A,MB);break;
case 0x84: /* $84 anda #@1 */
	CYCLES(2);FETCH();nValue = zValue = A = A & MB;overflowFlag = 0;break;
case 0x85: /* $85 bita #@1 */
	CYCLES(2);FETCH();overflowFlag = 0;nValue = zValue = A & MB;break;
case 0x86: /* $86 ldaa #@1 */
	CYCLES(2);FETCH();nValue = zValue = A = MB;overflowFlag = 0;break;
case 0x88: /* $88 eora #@1 */
	CYCLES(2);FETCH();nValue = zValue = A ^= MB;overflowFlag = 0;break;
case 0x89: /* $89 adca #@1 */
	CYCLES(2);FETCH();A = _CPUAdd8Bit(A,MB);break;
case 0x8a: /* $8a oraa #@1 */
	CYCLES(2);FETCH();nValue = zValue = A |= MB;overflowFlag = 0;break;
case 0x8b: /* $8b adda #@1 */
	CYCLES(2);FETCH();carryFlag = 0;A = _CPUAdd8Bit(A,MB);break;
case 0x8c: /* $8c cpx #@1@2 */
	CYCLES(4);FETCHWORD();_CPUCmp16Bit(IX,temp16);break;
case 0x8d: /* $8d bsr @1 */
	CYCLES(6);FETCH();bsrTemp = MB;PUSHWORD(PC);BRANCH(bsrTemp);break;
case 0x8e: /* $8e lds #@1@2 */
	CYCLES(3);FETCHWORD();SP = temp16;overflowFlag = 0;nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0x90: /* $90 suba @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;A = _CPUSub8Bit(A,MB);break;
case 0x91: /* $91 cmpa @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;_CPUSub8Bit(A,MB);break;
case 0x92: /* $92 sbca @1 */
	CYCLES(3);FETCH();MA = MB;READ();A = _CPUSub8Bit(A,MB);break;
case 0x94: /* $94 anda @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = A = A & MB;overflowFlag = 0;break;
case 0x95: /* $95 bita @1 */
	CYCLES(3);FETCH();MA = MB;READ();overflowFlag = 0;nValue = zValue = A & MB;break;
case 0x96: /* $96 ldaa @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = A = MB;overflowFlag = 0;break;
case 0x97: /* $97 staa @1 */
	CYCLES(3);FETCH();MA = MB;MB = A;WRITE();overflowFlag = 0;nValue = zValue = A;break;
case 0x98: /* $98 eora @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = A ^= MB;overflowFlag = 0;break;
case 0x99: /* $99 adca @1 */
	CYCLES(3);FETCH();MA = MB;READ();A = _CPUAdd8Bit(A,MB);break;
case 0x9a: /* $9a oraa @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = A |= MB;overflowFlag = 0;break;
case 0x9b: /* $9b adda @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;A = _CPUAdd8Bit(A,MB);break;
case 0x9c: /* $9c cpx @1 */
	CYCLES(5);FETCH();MA = MB;READWORD();_CPUCmp16Bit(IX,temp16);break;
case 0x9e: /* $9e lds @1 */
	CYCLES(4);FETCH();MA = MB;READWORD();SP = temp16;overflowFlag = 0;nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0x9f: /* $9f sts @1 */
	CYCLES(4);FETCH();MA = MB;MB = SP >> 8;WRITE();MA++;MB = SP;WRITE();nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0xa0: /* $a0 suba @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;A = _CPUSub8Bit(A,MB);break;
case 0xa1: /* $a1 cmpa @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;_CPUSub8Bit(A,MB);break;
case 0xa2: /* $a2 sbca @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();A = _CPUSub8Bit(A,MB);break;
case 0xa4: /* $a4 anda @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = A = A & MB;overflowFlag = 0;break;
case 0xa5: /* $a5 bita @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();overflowFlag = 0;nValue = zValue = A & MB;break;
case 0xa6: /* $a6 ldaa @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = A = MB;overflowFlag = 0;break;
case 0xa7: /* $a7 staa @1,x */
	CYCLES(4);FETCH();MA = MB + IX;MB = A;WRITE();overflowFlag = 0;nValue = zValue = A;break;
case 0xa8: /* $a8 eora @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = A ^= MB;overflowFlag = 0;break;
case 0xa9: /* $a9 adca @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();A = _CPUAdd8Bit(A,MB);break;
case 0xaa: /* $aa oraa @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = A |= MB;overflowFlag = 0;break;
case 0xab: /* $ab adda @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;A = _CPUAdd8Bit(A,MB);break;
case 0xac: /* $ac cpx @1,x */
	CYCLES(6);FETCH();MA = MB + IX;READWORD();_CPUCmp16Bit(IX,temp16);break;
case 0xad: /* $ad jsr @1,x */
	CYCLES(6);FETCH();MA = MB + IX;temp16 = MA;PUSHWORD(pc);PC = MA;break;
case 0xae: /* $ae lds @1,x */
	CYCLES(5);FETCH();MA = MB + IX;READWORD();SP = temp16;overflowFlag = 0;nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0xaf: /* $af sts @1,x */
	CYCLES(5);FETCH();MA = MB + IX;MB = SP >> 8;WRITE();MA++;MB = SP;WRITE();nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0xb0: /* $b0 suba @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;A = _CPUSub8Bit(A,MB);break;
case 0xb1: /* $b1 cmpa @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;_CPUSub8Bit(A,MB);break;
case 0xb2: /* $b2 sbca @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();A = _CPUSub8Bit(A,MB);break;
case 0xb4: /* $b4 anda @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = A = A & MB;overflowFlag = 0;break;
case 0xb5: /* $b5 bita @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();overflowFlag = 0;nValue = zValue = A & MB;break;
case 0xb6: /* $b6 ldaa @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = A = MB;overflowFlag = 0;break;
case 0xb7: /* $b7 staa @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;MB = A;WRITE();overflowFlag = 0;nValue = zValue = A;break;
case 0xb8: /* $b8 eora @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = A ^= MB;overflowFlag = 0;break;
case 0xb9: /* $b9 adca @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();A = _CPUAdd8Bit(A,MB);break;
case 0xba: /* $ba oraa @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = A |= MB;overflowFlag = 0;break;
case 0xbb: /* $bb adda @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;A = _CPUAdd8Bit(A,MB);break;
case 0xbc: /* $bc cpx @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;READWORD();_CPUCmp16Bit(IX,temp16);break;
case 0xbd: /* $bd jsr @1@2 */
	CYCLES(6);FETCHWORD();MA = temp16;temp16 = MA;PUSHWORD(pc);PC = MA;break;
case 0xbe: /* $be lds @1@2 */
	CYCLES(5);FETCHWORD();MA = temp16;READWORD();SP = temp16;overflowFlag = 0;nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0xbf: /* $bf sts @1@2 */
	CYCLES(5);FETCHWORD();MA = temp16;MB = SP >> 8;WRITE();MA++;MB = SP;WRITE();nValue = SP >> 8;zValue = (SP >> 8)|SP;break;
case 0xc0: /* $c0 subb #@1 */
	CYCLES(2);FETCH();carryFlag = 0;B = _CPUSub8Bit(B,MB);break;
case 0xc1: /* $c1 cmpb #@1 */
	CYCLES(2);FETCH();carryFlag = 0;_CPUSub8Bit(B,MB);break;
case 0xc2: /* $c2 sbcb #@1 */
	CYCLES(2);FETCH();B = _CPUSub8Bit(B,MB);break;
case 0xc4: /* $c4 andb #@1 */
	CYCLES(2);FETCH();nValue = zValue = B = B & MB;overflowFlag = 0;break;
case 0xc5: /* $c5 bitb #@1 */
	CYCLES(2);FETCH();overflowFlag = 0;nValue = zValue = B & MB;break;
case 0xc6: /* $c6 ldab #@1 */
	CYCLES(2);FETCH();nValue = zValue = B = MB;overflowFlag = 0;break;
case 0xc8: /* $c8 eorb #@1 */
	CYCLES(2);FETCH();nValue = zValue = B ^= MB;overflowFlag = 0;break;
case 0xc9: /* $c9 adcb #@1 */
	CYCLES(2);FETCH();B = _CPUAdd8Bit(B,MB);break;
case 0xca: /* $ca orab #@1 */
	CYCLES(2);FETCH();nValue = zValue = B |= MB;overflowFlag = 0;break;
case 0xcb: /* $cb addb #@1 */
	CYCLES(2);FETCH();carryFlag = 0;B = _CPUAdd8Bit(B,MB);break;
case 0xce: /* $ce ldx #@1@2 */
	CYCLES(3);FETCHWORD();IX = temp16;overflowFlag = 0;nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xd0: /* $d0 subb @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;B = _CPUSub8Bit(B,MB);break;
case 0xd1: /* $d1 cmpb @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;_CPUSub8Bit(B,MB);break;
case 0xd2: /* $d2 sbcb @1 */
	CYCLES(3);FETCH();MA = MB;READ();B = _CPUSub8Bit(B,MB);break;
case 0xd4: /* $d4 andb @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = B = B & MB;overflowFlag = 0;break;
case 0xd5: /* $d5 bitb @1 */
	CYCLES(3);FETCH();MA = MB;READ();overflowFlag = 0;nValue = zValue = B & MB;break;
case 0xd6: /* $d6 ldab @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = B = MB;overflowFlag = 0;break;
case 0xd7: /* $d7 stab @1 */
	CYCLES(3);FETCH();MA = MB;MB = B;WRITE();overflowFlag = 0;nValue = zValue = B;break;
case 0xd8: /* $d8 eorb @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = B ^= MB;overflowFlag = 0;break;
case 0xd9: /* $d9 adcb @1 */
	CYCLES(3);FETCH();MA = MB;READ();B = _CPUAdd8Bit(B,MB);break;
case 0xda: /* $da orab @1 */
	CYCLES(3);FETCH();MA = MB;READ();nValue = zValue = B |= MB;overflowFlag = 0;break;
case 0xdb: /* $db addb @1 */
	CYCLES(3);FETCH();MA = MB;READ();carryFlag = 0;B = _CPUAdd8Bit(B,MB);break;
case 0xde: /* $de ldx @1 */
	CYCLES(4);FETCH();MA = MB;READWORD();IX = temp16;overflowFlag = 0;nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xdf: /* $df stx @1 */
	CYCLES(4);FETCH();MA = MB;MB = IX >> 8;WRITE();MA++;MB = IX;WRITE();nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xe0: /* $e0 subb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;B = _CPUSub8Bit(B,MB);break;
case 0xe1: /* $e1 cmpb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;_CPUSub8Bit(B,MB);break;
case 0xe2: /* $e2 sbcb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();B = _CPUSub8Bit(B,MB);break;
case 0xe4: /* $e4 andb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = B = B & MB;overflowFlag = 0;break;
case 0xe5: /* $e5 bitb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();overflowFlag = 0;nValue = zValue = B & MB;break;
case 0xe6: /* $e6 ldab @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = B = MB;overflowFlag = 0;break;
case 0xe7: /* $e7 stab @1,x */
	CYCLES(4);FETCH();MA = MB + IX;MB = B;WRITE();overflowFlag = 0;nValue = zValue = B;break;
case 0xe8: /* $e8 eorb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = B ^= MB;overflowFlag = 0;break;
case 0xe9: /* $e9 adcb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();B = _CPUAdd8Bit(B,MB);break;
case 0xea: /* $ea orab @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();nValue = zValue = B |= MB;overflowFlag = 0;break;
case 0xeb: /* $eb addb @1,x */
	CYCLES(4);FETCH();MA = MB + IX;READ();carryFlag = 0;B = _CPUAdd8Bit(B,MB);break;
case 0xee: /* $ee ldx @1,x */
	CYCLES(5);FETCH();MA = MB + IX;READWORD();IX = temp16;overflowFlag = 0;nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xef: /* $ef stx @1,x */
	CYCLES(5);FETCH();MA = MB + IX;MB = IX >> 8;WRITE();MA++;MB = IX;WRITE();nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xf0: /* $f0 subb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;B = _CPUSub8Bit(B,MB);break;
case 0xf1: /* $f1 cmpb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;_CPUSub8Bit(B,MB);break;
case 0xf2: /* $f2 sbcb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();B = _CPUSub8Bit(B,MB);break;
case 0xf4: /* $f4 andb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = B = B & MB;overflowFlag = 0;break;
case 0xf5: /* $f5 bitb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();overflowFlag = 0;nValue = zValue = B & MB;break;
case 0xf6: /* $f6 ldab @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = B = MB;overflowFlag = 0;break;
case 0xf7: /* $f7 stab @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;MB = B;WRITE();overflowFlag = 0;nValue = zValue = B;break;
case 0xf8: /* $f8 eorb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = B ^= MB;overflowFlag = 0;break;
case 0xf9: /* $f9 adcb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();B = _CPUAdd8Bit(B,MB);break;
case 0xfa: /* $fa orab @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();nValue = zValue = B |= MB;overflowFlag = 0;break;
case 0xfb: /* $fb addb @1@2 */
	CYCLES(4);FETCHWORD();MA = temp16;READ();carryFlag = 0;B = _CPUAdd8Bit(B,MB);break;
case 0xfe: /* $fe ldx @1@2 */
	CYCLES(5);FETCHWORD();MA = temp16;READWORD();IX = temp16;overflowFlag = 0;nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
case 0xff: /* $ff stx @1@2 */
	CYCLES(5);FETCHWORD();MA = temp16;MB = IX >> 8;WRITE();MA++;MB = IX;WRITE();nValue = IX >> 8;zValue = (IX >> 8)|IX;break;
