case 0x00: /* $00 brk */
	CYCLES(7);brkCode();break;
case 0x01: /* $01 ora (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);sValue = zValue = a = a | READ(eac);break;
case 0x05: /* $05 ora @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = a = a | READ01(eac);break;
case 0x06: /* $06 asl @1 */
	CYCLES(5);eac = FETCH(); WRITE01(eac,aslCode(READ01(eac)));break;
case 0x08: /* $08 php */
	CYCLES(3);PUSH(constructFlagRegister());break;
case 0x09: /* $09 ora #@1 */
	CYCLES(2);sValue = zValue = a = a | FETCH();break;
case 0x0a: /* $0a asl a */
	CYCLES(2);a = aslCode(a);break;
case 0x0d: /* $0d ora @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = a = a | READ(eac);break;
case 0x0e: /* $0e asl @2 */
	CYCLES(6);FETCHWORD();eac = temp16; WRITE(eac,aslCode(READ(eac)));break;
case 0x10: /* $10 bpl @r */
	CYCLES(2);BRANCH((sValue & 0x80) == 0);break;
case 0x11: /* $11 ora (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;sValue = zValue = a = a | READ(eac);break;
case 0x15: /* $15 ora @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;sValue = zValue = a = a | READ01(eac);break;
case 0x16: /* $16 asl @1,x */
	CYCLES(6);eac = (FETCH()+x) & 0xFF; WRITE01(eac,aslCode(READ01(eac)));break;
case 0x18: /* $18 clc */
	CYCLES(2);carryFlag = 0;break;
case 0x19: /* $19 ora @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;sValue = zValue = a = a | READ(eac);break;
case 0x1d: /* $1d ora @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = a = a | READ(eac);break;
case 0x1e: /* $1e asl @2,x */
	CYCLES(6);FETCHWORD();eac = temp16+x; WRITE(eac,aslCode(READ(eac)));break;
case 0x20: /* $20 jsr @2 */
	CYCLES(6);FETCHWORD();eac = temp16;pc--;PUSH(pc >> 8);PUSH(pc & 0xFF);SETPC(eac);break;
case 0x21: /* $21 and (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8); a = a & READ(eac) ; sValue = zValue = a;break;
case 0x24: /* $24 bit @1 */
	CYCLES(2);eac = FETCH(); bitCode(READ01(eac));break;
case 0x25: /* $25 and @1 */
	CYCLES(3);eac = FETCH(); a = a & READ01(eac) ; sValue = zValue = a;break;
case 0x26: /* $26 rol @1 */
	CYCLES(3);eac = FETCH(); WRITE01(eac,rolCode(READ01(eac)));break;
case 0x28: /* $28 plp */
	CYCLES(4);explodeFlagRegister(POP());break;
case 0x29: /* $29 and #@1 */
	CYCLES(2); a = a & FETCH() ; sValue = zValue = a;break;
case 0x2a: /* $2a rol a */
	CYCLES(2);a = rolCode(a);break;
case 0x2c: /* $2c bit @2 */
	CYCLES(3);FETCHWORD();eac = temp16; bitCode(READ(eac));break;
case 0x2d: /* $2d and @2 */
	CYCLES(4);FETCHWORD();eac = temp16; a = a & READ(eac) ; sValue = zValue = a;break;
case 0x2e: /* $2e rol @2 */
	CYCLES(4);FETCHWORD();eac = temp16; WRITE(eac,rolCode(READ(eac)));break;
case 0x30: /* $30 bmi @r */
	CYCLES(2);BRANCH((sValue & 0x80) != 0);break;
case 0x31: /* $31 and (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y; a = a & READ(eac) ; sValue = zValue = a;break;
case 0x35: /* $35 and @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF; a = a & READ01(eac) ; sValue = zValue = a;break;
case 0x36: /* $36 rol @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF; WRITE01(eac,rolCode(READ01(eac)));break;
case 0x38: /* $38 sec */
	CYCLES(2);carryFlag = 1;break;
case 0x39: /* $39 and @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y; a = a & READ(eac) ; sValue = zValue = a;break;
case 0x3d: /* $3d and @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x; a = a & READ(eac) ; sValue = zValue = a;break;
case 0x3e: /* $3e rol @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x; WRITE(eac,rolCode(READ(eac)));break;
case 0x40: /* $40 rti */
	CYCLES(6);explodeFlagRegister(POP());pc = POP();pc = pc | (((WORD16)POP()) < 8);break;
case 0x41: /* $41 eor (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);sValue = zValue = a = a ^ READ(eac);break;
case 0x45: /* $45 eor @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = a = a ^ READ01(eac);break;
case 0x46: /* $46 lsr @1 */
	CYCLES(3);eac = FETCH(); WRITE01(eac,lsrCode(READ01(eac)));break;
case 0x48: /* $48 pha */
	CYCLES(3);PUSH(a);break;
case 0x49: /* $49 eor #@1 */
	CYCLES(2);sValue = zValue = a = a ^ FETCH();break;
case 0x4a: /* $4a lsr a */
	CYCLES(2);a = lsrCode(a);break;
case 0x4c: /* $4c jmp @2 */
	CYCLES(3);FETCHWORD();eac = temp16;SETPC(eac);break;
case 0x4d: /* $4d eor @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = a = a ^ READ(eac);break;
case 0x4e: /* $4e lsr @2 */
	CYCLES(4);FETCHWORD();eac = temp16; WRITE(eac,lsrCode(READ(eac)));break;
case 0x50: /* $50 bvc @r */
	CYCLES(2);BRANCH(overflowFlag == 0);break;
case 0x51: /* $51 eor (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;sValue = zValue = a = a ^ READ(eac);break;
case 0x55: /* $55 eor @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;sValue = zValue = a = a ^ READ01(eac);break;
case 0x56: /* $56 lsr @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF; WRITE01(eac,lsrCode(READ01(eac)));break;
case 0x58: /* $58 cli */
	CYCLES(2);interruptDisableFlag = 0;break;
case 0x59: /* $59 eor @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = a = a ^ READ(eac);break;
case 0x5d: /* $5d eor @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = a = a ^ READ(eac);break;
case 0x5e: /* $5e lsr @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x; WRITE(eac,lsrCode(READ(eac)));break;
case 0x60: /* $60 rts */
	CYCLES(6);pc = POP();pc = pc | (((WORD16)POP()) < 8);pc++;break;
case 0x61: /* $61 adc (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);sValue = zValue = a = add8Bit(a,READ(eac),decimalFlag);break;
case 0x65: /* $65 adc @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = a = add8Bit(a,READ01(eac),decimalFlag);break;
case 0x66: /* $66 ror @1 */
	CYCLES(3);eac = FETCH(); WRITE01(eac,rorCode(READ01(eac)));break;
case 0x68: /* $68 pla */
	CYCLES(4);a = sValue = zValue = POP();break;
case 0x69: /* $69 adc #@1 */
	CYCLES(2);sValue = zValue = a = add8Bit(a,FETCH(),decimalFlag);break;
case 0x6a: /* $6a ror a */
	CYCLES(2);a = rorCode(a);break;
case 0x6c: /* $6c jmp (@2) */
	CYCLES(5);FETCHWORD();eac = READWORD(temp16);SETPC(eac);break;
case 0x6d: /* $6d adc @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = a = add8Bit(a,READ(eac),decimalFlag);break;
case 0x6e: /* $6e ror @2 */
	CYCLES(4);FETCHWORD();eac = temp16; WRITE(eac,rorCode(READ(eac)));break;
case 0x70: /* $70 bvs @r */
	CYCLES(2);BRANCH(overflowFlag != 0);break;
case 0x71: /* $71 adc (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;sValue = zValue = a = add8Bit(a,READ(eac),decimalFlag);break;
case 0x75: /* $75 adc @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;sValue = zValue = a = add8Bit(a,READ01(eac),decimalFlag);break;
case 0x76: /* $76 ror @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF; WRITE01(eac,rorCode(READ01(eac)));break;
case 0x78: /* $78 sei */
	CYCLES(2);interruptDisableFlag = 1;break;
case 0x79: /* $79 adc @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;sValue = zValue = a = add8Bit(a,READ(eac),decimalFlag);break;
case 0x7d: /* $7d adc @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = a = add8Bit(a,READ(eac),decimalFlag);break;
case 0x7e: /* $7e ror @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x; WRITE(eac,rorCode(READ(eac)));break;
case 0x80: /* $80 bra @r */
	CYCLES(2);BRANCH(1);break;
case 0x81: /* $81 sta (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);WRITE(eac,a);break;
case 0x84: /* $84 sty @1 */
	CYCLES(3);eac = FETCH();WRITE01(eac,y);break;
case 0x85: /* $85 sta @1 */
	CYCLES(3);eac = FETCH();WRITE01(eac,a);break;
case 0x86: /* $86 stx @1 */
	CYCLES(3);eac = FETCH();WRITE01(eac,x);break;
case 0x88: /* $88 dey */
	CYCLES(2);sValue = zValue = --y;break;
case 0x8a: /* $8a txa */
	CYCLES(2);sValue = zValue = a = x;break;
case 0x8c: /* $8c sty @2 */
	CYCLES(4);FETCHWORD();eac = temp16;WRITE(eac,y);break;
case 0x8d: /* $8d sta @2 */
	CYCLES(4);FETCHWORD();eac = temp16;WRITE(eac,a);break;
case 0x8e: /* $8e stx @2 */
	CYCLES(4);FETCHWORD();eac = temp16;WRITE(eac,x);break;
case 0x90: /* $90 bcc @r */
	CYCLES(2);BRANCH(carryFlag == 0);break;
case 0x91: /* $91 sta (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;WRITE(eac,a);break;
case 0x94: /* $94 sty @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;WRITE01(eac,y);break;
case 0x95: /* $95 sta @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;WRITE01(eac,a);break;
case 0x96: /* $96 stx @1,y */
	CYCLES(4);eac = (FETCH()+y) & 0xFF;WRITE01(eac,x);break;
case 0x98: /* $98 tya */
	CYCLES(2);sValue = zValue = a = y;break;
case 0x99: /* $99 sta @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;WRITE(eac,a);break;
case 0x9a: /* $9a txs */
	CYCLES(2);s = x;break;
case 0x9d: /* $9d sta @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;WRITE(eac,a);break;
case 0xa0: /* $a0 ldy #@1 */
	CYCLES(2);y = sValue = zValue = FETCH();break;
case 0xa1: /* $a1 lda (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);a = sValue = zValue = READ(eac);break;
case 0xa2: /* $a2 ldx #@1 */
	CYCLES(2);x = sValue = zValue = FETCH();break;
case 0xa4: /* $a4 ldy @1 */
	CYCLES(3);eac = FETCH();y = sValue = zValue = READ01(eac);break;
case 0xa5: /* $a5 lda @1 */
	CYCLES(3);eac = FETCH();a = sValue = zValue = READ01(eac);break;
case 0xa6: /* $a6 ldx @1 */
	CYCLES(3);eac = FETCH();x = sValue = zValue = READ01(eac);break;
case 0xa8: /* $a8 tay */
	CYCLES(2);sValue = zValue = y = a;break;
case 0xa9: /* $a9 lda #@1 */
	CYCLES(2);a = sValue = zValue = FETCH();break;
case 0xaa: /* $aa tax */
	CYCLES(2);sValue = zValue = x = a;break;
case 0xac: /* $ac ldy @2 */
	CYCLES(4);FETCHWORD();eac = temp16;y = sValue = zValue = READ(eac);break;
case 0xad: /* $ad lda @2 */
	CYCLES(4);FETCHWORD();eac = temp16;a = sValue = zValue = READ(eac);break;
case 0xae: /* $ae ldx @2 */
	CYCLES(4);FETCHWORD();eac = temp16;x = sValue = zValue = READ(eac);break;
case 0xb0: /* $b0 bcs @r */
	CYCLES(2);BRANCH(carryFlag != 0);break;
case 0xb1: /* $b1 lda (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;a = sValue = zValue = READ(eac);break;
case 0xb4: /* $b4 ldy @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;y = sValue = zValue = READ01(eac);break;
case 0xb5: /* $b5 lda @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;a = sValue = zValue = READ01(eac);break;
case 0xb6: /* $b6 ldx @1,y */
	CYCLES(4);eac = (FETCH()+y) & 0xFF;x = sValue = zValue = READ01(eac);break;
case 0xb8: /* $b8 clv */
	CYCLES(2);overflowFlag = 0;break;
case 0xb9: /* $b9 lda @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;a = sValue = zValue = READ(eac);break;
case 0xba: /* $ba tsx */
	CYCLES(2);sValue = zValue = x = s;break;
case 0xbc: /* $bc ldy @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;y = sValue = zValue = READ(eac);break;
case 0xbd: /* $bd lda @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;a = sValue = zValue = READ(eac);break;
case 0xbe: /* $be ldx @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;x = sValue = zValue = READ(eac);break;
case 0xc0: /* $c0 cpy #@1 */
	CYCLES(2);sValue = zValue = sub8Bit(y,FETCH(),0);break;
case 0xc1: /* $c1 cmp */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);sValue = zValue = sub8Bit(a,READ(eac),0);break;
case 0xc4: /* $c4 cpy @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = sub8Bit(y,READ01(eac),0);break;
case 0xc5: /* $c5 cmp */
	CYCLES(3);eac = FETCH();sValue = zValue = sub8Bit(a,READ01(eac),0);break;
case 0xc6: /* $c6 dec @1 */
	CYCLES(5);eac = FETCH();sValue = zValue = READ01(eac)-1; WRITE01(eac,sValue);break;
case 0xc8: /* $c8 iny */
	CYCLES(2);sValue = zValue = ++y;break;
case 0xc9: /* $c9 cmp */
	CYCLES(2);sValue = zValue = sub8Bit(a,FETCH(),0);break;
case 0xca: /* $ca dex */
	CYCLES(2);sValue = zValue = --x;break;
case 0xcc: /* $cc cpy @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = sub8Bit(y,READ(eac),0);break;
case 0xcd: /* $cd cmp */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = sub8Bit(a,READ(eac),0);break;
case 0xce: /* $ce dec @2 */
	CYCLES(6);FETCHWORD();eac = temp16;sValue = zValue = READ(eac)-1; WRITE(eac,sValue);break;
case 0xd0: /* $d0 bne @r */
	CYCLES(2);BRANCH(zValue != 0);break;
case 0xd1: /* $d1 cmp */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;sValue = zValue = sub8Bit(a,READ(eac),0);break;
case 0xd5: /* $d5 cmp */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;sValue = zValue = sub8Bit(a,READ01(eac),0);break;
case 0xd6: /* $d6 dec @1,x */
	CYCLES(6);eac = (FETCH()+x) & 0xFF;sValue = zValue = READ01(eac)-1; WRITE01(eac,sValue);break;
case 0xd8: /* $d8 cld */
	CYCLES(2);decimalFlag = 0;break;
case 0xd9: /* $d9 cmp */
	CYCLES(4);FETCHWORD();eac = temp16+y;sValue = zValue = sub8Bit(a,READ(eac),0);break;
case 0xdd: /* $dd cmp */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = sub8Bit(a,READ(eac),0);break;
case 0xde: /* $de dec @1,x */
	CYCLES(6);eac = (FETCH()+x) & 0xFF;sValue = zValue = READ01(eac)-1; WRITE01(eac,sValue);break;
case 0xe0: /* $e0 cpx #@1 */
	CYCLES(2);sValue = zValue = sub8Bit(x,FETCH(),0);break;
case 0xe1: /* $e1 sbc (@1,x) */
	CYCLES(7);temp8 = FETCH()+x;eac = READWORD01(temp8);sValue = zValue = a = sub8Bit(a,READ(eac),decimalFlag);break;
case 0xe4: /* $e4 cpx @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = sub8Bit(x,READ01(eac),0);break;
case 0xe5: /* $e5 sbc @1 */
	CYCLES(3);eac = FETCH();sValue = zValue = a = sub8Bit(a,READ01(eac),decimalFlag);break;
case 0xe6: /* $e6 inc @1 */
	CYCLES(5);eac = FETCH();sValue = zValue = READ01(eac)+1; WRITE01(eac, sValue);break;
case 0xe8: /* $e8 inx */
	CYCLES(2);sValue = zValue = ++x;break;
case 0xe9: /* $e9 sbc #@1 */
	CYCLES(2);sValue = zValue = a = sub8Bit(a,FETCH(),decimalFlag);break;
case 0xea: /* $ea nop */
	CYCLES(2);{};break;
case 0xec: /* $ec cpx @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = sub8Bit(x,READ(eac),0);break;
case 0xed: /* $ed sbc @2 */
	CYCLES(4);FETCHWORD();eac = temp16;sValue = zValue = a = sub8Bit(a,READ(eac),decimalFlag);break;
case 0xee: /* $ee inc @2 */
	CYCLES(6);FETCHWORD();eac = temp16;sValue = zValue = READ(eac)+1; WRITE(eac, sValue);break;
case 0xf0: /* $f0 beq @r */
	CYCLES(2);BRANCH(zValue == 0);break;
case 0xf1: /* $f1 sbc (@1),y */
	CYCLES(6);temp8 = FETCH();eac = READWORD01(temp8)+y;sValue = zValue = a = sub8Bit(a,READ(eac),decimalFlag);break;
case 0xf5: /* $f5 sbc @1,x */
	CYCLES(4);eac = (FETCH()+x) & 0xFF;sValue = zValue = a = sub8Bit(a,READ01(eac),decimalFlag);break;
case 0xf6: /* $f6 inc @1,x */
	CYCLES(6);eac = (FETCH()+x) & 0xFF;sValue = zValue = READ01(eac)+1; WRITE01(eac, sValue);break;
case 0xf8: /* $f8 sed */
	CYCLES(2);decimalFlag = 1;break;
case 0xf9: /* $f9 sbc @2,y */
	CYCLES(4);FETCHWORD();eac = temp16+y;sValue = zValue = a = sub8Bit(a,READ(eac),decimalFlag);break;
case 0xfd: /* $fd sbc @2,x */
	CYCLES(4);FETCHWORD();eac = temp16+x;sValue = zValue = a = sub8Bit(a,READ(eac),decimalFlag);break;
case 0xfe: /* $fe inc @2,x */
	CYCLES(6);FETCHWORD();eac = temp16+x;sValue = zValue = READ(eac)+1; WRITE(eac, sValue);break;
