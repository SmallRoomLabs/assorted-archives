case 0x01: /* $01 nop */
	CYCLES(2);{};break;
case 0x06: /* $06 tap */
	CYCLES(2);putCCR(a);break;
case 0x07: /* $07 tpa */
	CYCLES(2);a = getCCR();break;
case 0x08: /* $08 inx */
	CYCLES(3);ix++;zValue = ix | (ix >> 8);break;
case 0x09: /* $09 dex */
	CYCLES(3);ix--;zValue = ix | (ix >> 8);break;
case 0x0a: /* $0a clv */
	CYCLES(2);overflow = 0;break;
case 0x0b: /* $0b sev */
	CYCLES(2);overflow = 1;break;
case 0x0c: /* $0c clc */
	CYCLES(2);carry = 0;break;
case 0x0d: /* $0d sec */
	CYCLES(2);carry = 1;break;
case 0x0e: /* $0e cli */
	CYCLES(2);interruptMask = 0;break;
case 0x0f: /* $0f sei */
	CYCLES(2);interruptMask = 1;break;
case 0x10: /* $10 sba */
	CYCLES(2);carry = 0;a = ALUSub8BitWithCarry(a,b);break;
case 0x11: /* $11 cba */
	CYCLES(2);carry = 0;ALUSub8BitWithCarry(a,b);break;
case 0x16: /* $16 tab */
	CYCLES(2);nValue = zValue = b = a;overflow = 0;break;
case 0x17: /* $17 tba */
	CYCLES(2);nValue = zValue = a = b;overflow = 0;break;
case 0x19: /* $19 daa */
	CYCLES(2);ALUDecimalAdjust();break;
case 0x1b: /* $1b aba */
	CYCLES(2);carry = 0;a = ALUAdd8BitWithCarry(a,b);break;
case 0x20: /* $20 bra @1 */
	CYCLES(3);temp8 = fetchByte();BRANCH(temp8);break;
case 0x22: /* $22 bhi @1 */
	CYCLES(3);BRANCHIF(carry == 0 && zValue != 0);break;
case 0x23: /* $23 bls @1 */
	CYCLES(3);BRANCHIF(!(carry == 0 && zValue != 0));break;
case 0x24: /* $24 bcc @1 */
	CYCLES(3);BRANCHIF(carry == 0);break;
case 0x25: /* $25 bcs @1 */
	CYCLES(3);BRANCHIF(carry != 0);break;
case 0x26: /* $26 bne @1 */
	CYCLES(3);BRANCHIF(zValue != 0);break;
case 0x27: /* $27 beq @1 */
	CYCLES(3);BRANCHIF(zValue == 0);break;
case 0x28: /* $28 bvc @1 */
	CYCLES(3);BRANCHIF(overflow == 0);break;
case 0x29: /* $29 bvs @1 */
	CYCLES(3);BRANCHIF(overflow != 0);break;
case 0x2a: /* $2a bpl @1 */
	CYCLES(3);BRANCHIF((nValue & 0x80) == 0);break;
case 0x2b: /* $2b bmi @1 */
	CYCLES(3);BRANCHIF((nValue & 0x80) != 0);break;
case 0x2c: /* $2c bge @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) == overflow);break;
case 0x2d: /* $2d blt @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) != overflow);break;
case 0x2e: /* $2e bgt @1 */
	CYCLES(3);BRANCHIF((nValue >> 7) == overflow && zValue != 0);break;
case 0x2f: /* $2f ble @1 */
	CYCLES(3);BRANCHIF(!((nValue >> 7) == overflow && zValue != 0));break;
case 0x30: /* $30 tsx */
	CYCLES(3);ix = sp + 1;break;
case 0x31: /* $31 ins */
	CYCLES(3);sp++;break;
case 0x32: /* $32 pula */
	CYCLES(4);a = PULLBYTE();break;
case 0x33: /* $33 pulb */
	CYCLES(4);b = PULLBYTE();break;
case 0x34: /* $34 des */
	CYCLES(3);sp--;break;
case 0x35: /* $35 txs */
	CYCLES(3);sp = ix - 1;break;
case 0x36: /* $36 psha */
	CYCLES(3);PUSHBYTE(a);break;
case 0x37: /* $37 pshb */
	CYCLES(3);PUSHBYTE(b);break;
case 0x39: /* $39 rts */
	CYCLES(5);PULLWORDTEMP16();setProgramCounter(temp16);break;
case 0x3b: /* $3b rti */
	CYCLES(10);putCCR(PULLBYTE());b = PULLBYTE();a = PULLBYTE();PULLWORDTEMP16();ix = temp16;PULLWORDTEMP16();setProgramCounter(temp16);break;
case 0x3f: /* $3f swi */
	CYCLES(12);softwareInterrupt();break;
case 0x40: /* $40 nega */
	CYCLES(2);carry = 0;a = ALUSub8BitWithCarry(0,a);break;
case 0x43: /* $43 coma */
	CYCLES(2);carry = 1;overflow = 0;nValue = zValue = a = a ^ 0xFF;break;
case 0x44: /* $44 lsra */
	CYCLES(2);a = ALULSRShift(a);break;
case 0x46: /* $46 rora */
	CYCLES(2);a = ALURORRotate(a);break;
case 0x47: /* $47 asra */
	CYCLES(2);a = ALUASRShift(a);break;
case 0x48: /* $48 asla */
	CYCLES(2);a = ALUASLShift(a);break;
case 0x49: /* $49 rola */
	CYCLES(2);a = ALUROLRotate(a);break;
case 0x4a: /* $4a deca */
	CYCLES(2);overflow = (a == 0x80) ? 1 : 0;nValue = zValue = --a;break;
case 0x4c: /* $4c inca */
	CYCLES(2);overflow = (a == 0x7F) ? 1 : 0;nValue = zValue = ++a;break;
case 0x4d: /* $4d tsta */
	CYCLES(2);carry = overflow = 0;zValue = nValue = a;break;
case 0x4f: /* $4f clra */
	CYCLES(2);a = nValue = zValue = overflow = carry = 0;break;
case 0x50: /* $50 negb */
	CYCLES(2);carry = 0;b = ALUSub8BitWithCarry(0,b);break;
case 0x53: /* $53 comb */
	CYCLES(2);carry = 1;overflow = 0;nValue = zValue = b = b ^ 0xFF;break;
case 0x54: /* $54 lsrb */
	CYCLES(2);b = ALULSRShift(b);break;
case 0x56: /* $56 rorb */
	CYCLES(2);b = ALURORRotate(b);break;
case 0x57: /* $57 asrb */
	CYCLES(2);b = ALUASRShift(b);break;
case 0x58: /* $58 aslb */
	CYCLES(2);b = ALUASLShift(b);break;
case 0x59: /* $59 rolb */
	CYCLES(2);b = ALUROLRotate(b);break;
case 0x5a: /* $5a decb */
	CYCLES(2);overflow = (b == 0x80) ? 1 : 0;nValue = zValue = --b;break;
case 0x5c: /* $5c incb */
	CYCLES(2);overflow = (b == 0x7F) ? 1 : 0;nValue = zValue = ++b;break;
case 0x5d: /* $5d tstb */
	CYCLES(2);carry = overflow = 0;zValue = nValue = b;break;
case 0x5f: /* $5f clrb */
	CYCLES(2);b = nValue = zValue = overflow = carry = 0;break;
case 0x60: /* $60 neg */
	CYCLES(6);eac = fetchByte()+ix;carry = 0;writeByte(eac,ALUSub8BitWithCarry(0,readByte(eac)));break;
case 0x63: /* $63 com */
	CYCLES(6);eac = fetchByte()+ix;carry = 1;overflow = 0;nValue = zValue = readByte(eac) ^ 0xFF;writeByte(eac,zValue);break;
case 0x64: /* $64 lsr */
	CYCLES(6);eac = fetchByte()+ix;writeByte(eac,ALULSRShift(readByte(eac)));break;
case 0x66: /* $66 ror */
	CYCLES(6);eac = fetchByte()+ix;writeByte(eac,ALURORRotate(readByte(eac)));break;
case 0x67: /* $67 asr */
	CYCLES(6);eac = fetchByte()+ix;writeByte(eac,ALUASRShift(readByte(eac)));break;
case 0x68: /* $68 asl */
	CYCLES(6);eac = fetchByte()+ix;writeByte(eac,ALUASLShift(readByte(eac)));break;
case 0x69: /* $69 rol */
	CYCLES(6);eac = fetchByte()+ix;writeByte(eac,ALUROLRotate(readByte(eac)));break;
case 0x6a: /* $6a dec */
	CYCLES(6);eac = fetchByte()+ix;temp8 = readByte(eac);overflow = (temp8 == 0x80) ? 1 : 0;nValue = zValue = --temp8;writeByte(eac,temp8);break;
case 0x6c: /* $6c inc */
	CYCLES(6);eac = fetchByte()+ix;temp8 = readByte(eac);overflow = (temp8 == 0x7F) ? 1 : 0;nValue = zValue = ++temp8;writeByte(eac,temp8);break;
case 0x6d: /* $6d tst */
	CYCLES(6);eac = fetchByte()+ix;carry = overflow = 0;zValue = nValue = readByte(eac);break;
case 0x6e: /* $6e jmp */
	CYCLES(3);eac = fetchByte()+ix;setProgramCounter(eac);break;
case 0x6f: /* $6f clr */
	CYCLES(6);eac = fetchByte()+ix;nValue = zValue = overflow = carry = 0;writeByte(eac,0);break;
case 0x70: /* $70 neg */
	CYCLES(6);fetchWordTemp16();eac = temp16;carry = 0;writeByte(eac,ALUSub8BitWithCarry(0,readByte(eac)));break;
case 0x73: /* $73 com */
	CYCLES(6);fetchWordTemp16();eac = temp16;carry = 1;overflow = 0;nValue = zValue = readByte(eac) ^ 0xFF;writeByte(eac,zValue);break;
case 0x74: /* $74 lsr */
	CYCLES(6);fetchWordTemp16();eac = temp16;writeByte(eac,ALULSRShift(readByte(eac)));break;
case 0x76: /* $76 ror */
	CYCLES(6);fetchWordTemp16();eac = temp16;writeByte(eac,ALURORRotate(readByte(eac)));break;
case 0x77: /* $77 asr */
	CYCLES(6);fetchWordTemp16();eac = temp16;writeByte(eac,ALUASRShift(readByte(eac)));break;
case 0x78: /* $78 asl */
	CYCLES(6);fetchWordTemp16();eac = temp16;writeByte(eac,ALUASLShift(readByte(eac)));break;
case 0x79: /* $79 rol */
	CYCLES(6);fetchWordTemp16();eac = temp16;writeByte(eac,ALUROLRotate(readByte(eac)));break;
case 0x7a: /* $7a dec */
	CYCLES(6);fetchWordTemp16();eac = temp16;temp8 = readByte(eac);overflow = (temp8 == 0x80) ? 1 : 0;nValue = zValue = --temp8;writeByte(eac,temp8);break;
case 0x7c: /* $7c inc */
	CYCLES(6);fetchWordTemp16();eac = temp16;temp8 = readByte(eac);overflow = (temp8 == 0x7F) ? 1 : 0;nValue = zValue = ++temp8;writeByte(eac,temp8);break;
case 0x7d: /* $7d tst */
	CYCLES(6);fetchWordTemp16();eac = temp16;carry = overflow = 0;zValue = nValue = readByte(eac);break;
case 0x7e: /* $7e jmp */
	CYCLES(3);fetchWordTemp16();eac = temp16;setProgramCounter(eac);break;
case 0x7f: /* $7f clr */
	CYCLES(6);fetchWordTemp16();eac = temp16;nValue = zValue = overflow = carry = 0;writeByte(eac,0);break;
case 0x80: /* $80 suba */
	CYCLES(2);carry = 0;a = ALUSub8BitWithCarry(a,fetchByte());break;
case 0x81: /* $81 cmpa */
	CYCLES(2);carry = 0;ALUSub8BitWithCarry(a,fetchByte());break;
case 0x82: /* $82 sbca */
	CYCLES(2);a = ALUSub8BitWithCarry(a,fetchByte());break;
case 0x84: /* $84 anda */
	CYCLES(2);nValue = zValue = a = a & fetchByte();overflow = 0;break;
case 0x85: /* $85 bita */
	CYCLES(2);overflow = 0;nValue = zValue = a & fetchByte();break;
case 0x86: /* $86 ldaa */
	CYCLES(2);nValue = zValue = a = fetchByte();overflow = 0;break;
case 0x88: /* $88 eora */
	CYCLES(2);nValue = zValue = a ^= fetchByte();overflow = 0;break;
case 0x89: /* $89 adca */
	CYCLES(2);a = ALUAdd8BitWithCarry(a,fetchByte());break;
case 0x8a: /* $8a oraa */
	CYCLES(2);nValue = zValue = a |= fetchByte();overflow = 0;break;
case 0x8b: /* $8b adda */
	CYCLES(2);carry = 0;a = ALUAdd8BitWithCarry(a,fetchByte());break;
case 0x8c: /* $8c cpx #@1@2 */
	CYCLES(4);FETCHWORDTEMP16();ALUCompare16Bit(ix,temp16);break;
case 0x8d: /* $8d bsr @1 */
	CYCLES(6);temp8 = fetchByte();PUSHWORD(pc);BRANCH(temp8);break;
case 0x8e: /* $8e lds #@1@2 */
	CYCLES(3);FETCHWORDTEMP16();sp = temp16;overflow = 0;nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0x90: /* $90 suba */
	CYCLES(3);eac = fetchByte();carry = 0;a = ALUSub8BitWithCarry(a,readBytePage0(eac));break;
case 0x91: /* $91 cmpa */
	CYCLES(3);eac = fetchByte();carry = 0;ALUSub8BitWithCarry(a,readBytePage0(eac));break;
case 0x92: /* $92 sbca */
	CYCLES(3);eac = fetchByte();a = ALUSub8BitWithCarry(a,readBytePage0(eac));break;
case 0x94: /* $94 anda */
	CYCLES(3);eac = fetchByte();nValue = zValue = a = a & readBytePage0(eac);overflow = 0;break;
case 0x95: /* $95 bita */
	CYCLES(3);eac = fetchByte();overflow = 0;nValue = zValue = a & readBytePage0(eac);break;
case 0x96: /* $96 ldaa */
	CYCLES(3);eac = fetchByte();nValue = zValue = a = readBytePage0(eac);overflow = 0;break;
case 0x97: /* $97 staa */
	CYCLES(3);eac = fetchByte();writeBytePage0(eac,a);overflow = 0;nValue = zValue = a;break;
case 0x98: /* $98 eora */
	CYCLES(3);eac = fetchByte();nValue = zValue = a ^= readBytePage0(eac);overflow = 0;break;
case 0x99: /* $99 adca */
	CYCLES(3);eac = fetchByte();a = ALUAdd8BitWithCarry(a,readBytePage0(eac));break;
case 0x9a: /* $9a oraa */
	CYCLES(3);eac = fetchByte();nValue = zValue = a |= readBytePage0(eac);overflow = 0;break;
case 0x9b: /* $9b adda */
	CYCLES(3);eac = fetchByte();carry = 0;a = ALUAdd8BitWithCarry(a,readBytePage0(eac));break;
case 0x9c: /* $9c cpx */
	CYCLES(5);eac = fetchByte();READWORDTEMP16(eac);ALUCompare16Bit(ix,temp16);break;
case 0x9e: /* $9e lds */
	CYCLES(4);eac = fetchByte();READWORDTEMP16(eac);sp = temp16;overflow = 0;nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0x9f: /* $9f sts */
	CYCLES(4);eac = fetchByte();writeBytePage0(eac,sp >> 8);writeBytePage0(eac+1,sp);nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0xa0: /* $a0 suba */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;a = ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xa1: /* $a1 cmpa */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xa2: /* $a2 sbca */
	CYCLES(4);eac = fetchByte()+ix;a = ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xa4: /* $a4 anda */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = a = a & readByte(eac);overflow = 0;break;
case 0xa5: /* $a5 bita */
	CYCLES(4);eac = fetchByte()+ix;overflow = 0;nValue = zValue = a & readByte(eac);break;
case 0xa6: /* $a6 ldaa */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = a = readByte(eac);overflow = 0;break;
case 0xa7: /* $a7 staa */
	CYCLES(4);eac = fetchByte()+ix;writeByte(eac,a);overflow = 0;nValue = zValue = a;break;
case 0xa8: /* $a8 eora */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = a ^= readByte(eac);overflow = 0;break;
case 0xa9: /* $a9 adca */
	CYCLES(4);eac = fetchByte()+ix;a = ALUAdd8BitWithCarry(a,readByte(eac));break;
case 0xaa: /* $aa oraa */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = a |= readByte(eac);overflow = 0;break;
case 0xab: /* $ab adda */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;a = ALUAdd8BitWithCarry(a,readByte(eac));break;
case 0xac: /* $ac cpx */
	CYCLES(6);eac = fetchByte()+ix;READWORDTEMP16(eac);ALUCompare16Bit(ix,temp16);break;
case 0xad: /* $ad jsr */
	CYCLES(6);eac = fetchByte()+ix;PUSHWORD(pc);setProgramCounter(eac);break;
case 0xae: /* $ae lds */
	CYCLES(5);eac = fetchByte()+ix;READWORDTEMP16(eac);sp = temp16;overflow = 0;nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0xaf: /* $af sts */
	CYCLES(5);eac = fetchByte()+ix;writeByte(eac,sp >> 8);writeByte(eac+1,sp);nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0xb0: /* $b0 suba */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;a = ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xb1: /* $b1 cmpa */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xb2: /* $b2 sbca */
	CYCLES(4);fetchWordTemp16();eac = temp16;a = ALUSub8BitWithCarry(a,readByte(eac));break;
case 0xb4: /* $b4 anda */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = a = a & readByte(eac);overflow = 0;break;
case 0xb5: /* $b5 bita */
	CYCLES(4);fetchWordTemp16();eac = temp16;overflow = 0;nValue = zValue = a & readByte(eac);break;
case 0xb6: /* $b6 ldaa */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = a = readByte(eac);overflow = 0;break;
case 0xb7: /* $b7 staa */
	CYCLES(4);fetchWordTemp16();eac = temp16;writeByte(eac,a);overflow = 0;nValue = zValue = a;break;
case 0xb8: /* $b8 eora */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = a ^= readByte(eac);overflow = 0;break;
case 0xb9: /* $b9 adca */
	CYCLES(4);fetchWordTemp16();eac = temp16;a = ALUAdd8BitWithCarry(a,readByte(eac));break;
case 0xba: /* $ba oraa */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = a |= readByte(eac);overflow = 0;break;
case 0xbb: /* $bb adda */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;a = ALUAdd8BitWithCarry(a,readByte(eac));break;
case 0xbc: /* $bc cpx */
	CYCLES(6);fetchWordTemp16();eac = temp16;READWORDTEMP16(eac);ALUCompare16Bit(ix,temp16);break;
case 0xbd: /* $bd jsr */
	CYCLES(6);fetchWordTemp16();eac = temp16;PUSHWORD(pc);setProgramCounter(eac);break;
case 0xbe: /* $be lds */
	CYCLES(5);fetchWordTemp16();eac = temp16;READWORDTEMP16(eac);sp = temp16;overflow = 0;nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0xbf: /* $bf sts */
	CYCLES(5);fetchWordTemp16();eac = temp16;writeByte(eac,sp >> 8);writeByte(eac+1,sp);nValue = sp >> 8;zValue = (sp >> 8)|sp;break;
case 0xc0: /* $c0 subb */
	CYCLES(2);carry = 0;b = ALUSub8BitWithCarry(b,fetchByte());break;
case 0xc1: /* $c1 cmpb */
	CYCLES(2);carry = 0;ALUSub8BitWithCarry(b,fetchByte());break;
case 0xc2: /* $c2 sbcb */
	CYCLES(2);b = ALUSub8BitWithCarry(b,fetchByte());break;
case 0xc4: /* $c4 andb */
	CYCLES(2);nValue = zValue = b = b & fetchByte();overflow = 0;break;
case 0xc5: /* $c5 bitb */
	CYCLES(2);overflow = 0;nValue = zValue = b & fetchByte();break;
case 0xc6: /* $c6 ldab */
	CYCLES(2);nValue = zValue = b = fetchByte();overflow = 0;break;
case 0xc8: /* $c8 eorb */
	CYCLES(2);nValue = zValue = b ^= fetchByte();overflow = 0;break;
case 0xc9: /* $c9 adcb */
	CYCLES(2);b = ALUAdd8BitWithCarry(b,fetchByte());break;
case 0xca: /* $ca orab */
	CYCLES(2);nValue = zValue = b |= fetchByte();overflow = 0;break;
case 0xcb: /* $cb addb */
	CYCLES(2);carry = 0;b = ALUAdd8BitWithCarry(b,fetchByte());break;
case 0xce: /* $ce ldx #@1@2 */
	CYCLES(3);FETCHWORDTEMP16();ix = temp16;overflow = 0;nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xd0: /* $d0 subb */
	CYCLES(3);eac = fetchByte();carry = 0;b = ALUSub8BitWithCarry(b,readBytePage0(eac));break;
case 0xd1: /* $d1 cmpb */
	CYCLES(3);eac = fetchByte();carry = 0;ALUSub8BitWithCarry(b,readBytePage0(eac));break;
case 0xd2: /* $d2 sbcb */
	CYCLES(3);eac = fetchByte();b = ALUSub8BitWithCarry(b,readBytePage0(eac));break;
case 0xd4: /* $d4 andb */
	CYCLES(3);eac = fetchByte();nValue = zValue = b = b & readBytePage0(eac);overflow = 0;break;
case 0xd5: /* $d5 bitb */
	CYCLES(3);eac = fetchByte();overflow = 0;nValue = zValue = b & readBytePage0(eac);break;
case 0xd6: /* $d6 ldab */
	CYCLES(3);eac = fetchByte();nValue = zValue = b = readBytePage0(eac);overflow = 0;break;
case 0xd7: /* $d7 stab */
	CYCLES(3);eac = fetchByte();writeBytePage0(eac,b);overflow = 0;nValue = zValue = b;break;
case 0xd8: /* $d8 eorb */
	CYCLES(3);eac = fetchByte();nValue = zValue = b ^= readBytePage0(eac);overflow = 0;break;
case 0xd9: /* $d9 adcb */
	CYCLES(3);eac = fetchByte();b = ALUAdd8BitWithCarry(b,readBytePage0(eac));break;
case 0xda: /* $da orab */
	CYCLES(3);eac = fetchByte();nValue = zValue = b |= readBytePage0(eac);overflow = 0;break;
case 0xdb: /* $db addb */
	CYCLES(3);eac = fetchByte();carry = 0;b = ALUAdd8BitWithCarry(b,readBytePage0(eac));break;
case 0xde: /* $de ldx */
	CYCLES(4);eac = fetchByte();READWORDTEMP16(eac);ix = temp16;overflow = 0;nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xdf: /* $df stx */
	CYCLES(4);eac = fetchByte();writeBytePage0(eac,ix >> 8);writeBytePage0(eac+1,ix);nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xe0: /* $e0 subb */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;b = ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xe1: /* $e1 cmpb */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xe2: /* $e2 sbcb */
	CYCLES(4);eac = fetchByte()+ix;b = ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xe4: /* $e4 andb */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = b = b & readByte(eac);overflow = 0;break;
case 0xe5: /* $e5 bitb */
	CYCLES(4);eac = fetchByte()+ix;overflow = 0;nValue = zValue = b & readByte(eac);break;
case 0xe6: /* $e6 ldab */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = b = readByte(eac);overflow = 0;break;
case 0xe7: /* $e7 stab */
	CYCLES(4);eac = fetchByte()+ix;writeByte(eac,b);overflow = 0;nValue = zValue = b;break;
case 0xe8: /* $e8 eorb */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = b ^= readByte(eac);overflow = 0;break;
case 0xe9: /* $e9 adcb */
	CYCLES(4);eac = fetchByte()+ix;b = ALUAdd8BitWithCarry(b,readByte(eac));break;
case 0xea: /* $ea orab */
	CYCLES(4);eac = fetchByte()+ix;nValue = zValue = b |= readByte(eac);overflow = 0;break;
case 0xeb: /* $eb addb */
	CYCLES(4);eac = fetchByte()+ix;carry = 0;b = ALUAdd8BitWithCarry(b,readByte(eac));break;
case 0xee: /* $ee ldx */
	CYCLES(5);eac = fetchByte()+ix;READWORDTEMP16(eac);ix = temp16;overflow = 0;nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xef: /* $ef stx */
	CYCLES(5);eac = fetchByte()+ix;writeByte(eac,ix >> 8);writeByte(eac+1,ix);nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xf0: /* $f0 subb */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;b = ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xf1: /* $f1 cmpb */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xf2: /* $f2 sbcb */
	CYCLES(4);fetchWordTemp16();eac = temp16;b = ALUSub8BitWithCarry(b,readByte(eac));break;
case 0xf4: /* $f4 andb */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = b = b & readByte(eac);overflow = 0;break;
case 0xf5: /* $f5 bitb */
	CYCLES(4);fetchWordTemp16();eac = temp16;overflow = 0;nValue = zValue = b & readByte(eac);break;
case 0xf6: /* $f6 ldab */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = b = readByte(eac);overflow = 0;break;
case 0xf7: /* $f7 stab */
	CYCLES(4);fetchWordTemp16();eac = temp16;writeByte(eac,b);overflow = 0;nValue = zValue = b;break;
case 0xf8: /* $f8 eorb */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = b ^= readByte(eac);overflow = 0;break;
case 0xf9: /* $f9 adcb */
	CYCLES(4);fetchWordTemp16();eac = temp16;b = ALUAdd8BitWithCarry(b,readByte(eac));break;
case 0xfa: /* $fa orab */
	CYCLES(4);fetchWordTemp16();eac = temp16;nValue = zValue = b |= readByte(eac);overflow = 0;break;
case 0xfb: /* $fb addb */
	CYCLES(4);fetchWordTemp16();eac = temp16;carry = 0;b = ALUAdd8BitWithCarry(b,readByte(eac));break;
case 0xfe: /* $fe ldx */
	CYCLES(5);fetchWordTemp16();eac = temp16;READWORDTEMP16(eac);ix = temp16;overflow = 0;nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
case 0xff: /* $ff stx */
	CYCLES(5);fetchWordTemp16();eac = temp16;writeByte(eac,ix >> 8);writeByte(eac+1,ix);nValue = ix >> 8;zValue = (ix >> 8)|ix;break;
