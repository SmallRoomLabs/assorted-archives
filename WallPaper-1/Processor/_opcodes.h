case 0x00: /*** HALT ***/
	CYCLES = CYCLES + 5;
	break;
case 0x01: /*** XAE ***/
	OPERAND = AC;
	AC = EX;
	EX = OPERAND;
	CYCLES = CYCLES + 7;
	break;
case 0x02: /*** CCL ***/
	SR = SR & 0x7F;
	CYCLES = CYCLES + 5;
	break;
case 0x03: /*** SCL ***/
	SR = SR | 0x80;
	CYCLES = CYCLES + 5;
	break;
case 0x04: /*** DINT ***/
	SR = SR & 0xF7;
	CYCLES = CYCLES + 6;
	break;
case 0x05: /*** IEN ***/
	SR = SR | 0x08;
	CYCLES = CYCLES + 6;
	break;
case 0x06: /*** CSA ***/
	SR = (SR & 0xCF) | SCMPReadSenseFlags();
	CYCLES = CYCLES + 5;
	break;
case 0x07: /*** CAS ***/
	SR = AC;
	CYCLES = CYCLES + 6;
	break;
case 0x08: /*** NOP ***/
	CYCLES = CYCLES + 5;
	break;
case 0x19: /*** SIO ***/
	EX = (EX >> 1);
	CYCLES = CYCLES + 5;
	break;
case 0x1c: /*** SR ***/
	AC = (AC >> 1);
	CYCLES = CYCLES + 5;
	break;
case 0x1d: /*** SRL ***/
	AC = (AC >> 1) | (SR & 0x80);
	CYCLES = CYCLES + 5;
	break;
case 0x1e: /*** RR ***/
	AC = ((AC >> 1) | (AC << 7)) & 0xFF;
	CYCLES = CYCLES + 5;
	break;
case 0x1f: /*** RRL ***/
	OPERAND = AC & 1;
	AC = ((AC >> 1) | (SR & 0x80);
	SR = (SR & 0x7F) | (OPERAND << 7);
	CYCLES = CYCLES + 5;
	break;
case 0x30: /*** XPAL P0 ***/
	OPERAND = P0 & 0xFF;
	P0 = (P0 & 0xFF00)|AC;
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x31: /*** XPAL P1 ***/
	OPERAND = P1 & 0xFF;
	P1 = (P1 & 0xFF00)|AC;
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x32: /*** XPAL P2 ***/
	OPERAND = P2 & 0xFF;
	P2 = (P2 & 0xFF00)|AC;
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x33: /*** XPAL P3 ***/
	OPERAND = P3 & 0xFF;
	P3 = (P3 & 0xFF00)|AC;
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x34: /*** XPAH P0 ***/
	OPERAND = (P0 >> 8) & 0xFF;
	P0 = (P0 & 0x00FF)|(((WORD16)AC) << 8);
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x35: /*** XPAH P1 ***/
	OPERAND = (P1 >> 8) & 0xFF;
	P1 = (P1 & 0x00FF)|(((WORD16)AC) << 8);
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x36: /*** XPAH P2 ***/
	OPERAND = (P2 >> 8) & 0xFF;
	P2 = (P2 & 0x00FF)|(((WORD16)AC) << 8);
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x37: /*** XPAH P3 ***/
	OPERAND = (P3 >> 8) & 0xFF;
	P3 = (P3 & 0x00FF)|(((WORD16)AC) << 8);
	AC = OPERAND;
	CYCLES = CYCLES + 8;
	break;
case 0x3c: /*** XPPC P0 ***/
	CYCLES = CYCLES + 7;
	break;
case 0x3d: /*** XPPC P1 ***/
	TEMP16 = P1;
	P1 = P0;
	P0 = TEMP16;
	CYCLES = CYCLES + 7;
	break;
case 0x3e: /*** XPPC P2 ***/
	TEMP16 = P2;
	P2 = P0;
	P0 = TEMP16;
	CYCLES = CYCLES + 7;
	break;
case 0x3f: /*** XPPC P3 ***/
	TEMP16 = P3;
	P3 = P0;
	P0 = TEMP16;
	CYCLES = CYCLES + 7;
	break;
case 0x40: /*** LDE ***/
	AC = EX;
	CYCLES = CYCLES + 6;
	break;
case 0x50: /*** ANE ***/
	AC = AC & EX;
	CYCLES = CYCLES + 6;
	break;
case 0x58: /*** ORE ***/
	AC = AC | EX;
	CYCLES = CYCLES + 6;
	break;
case 0x60: /*** XRE ***/
	AC = AC ^ EX;
	CYCLES = CYCLES + 6;
	break;
case 0x68: /*** DAE ***/
	AC = _SCMPDecimalAdd(AC,EX);
	CYCLES = CYCLES + 11;
	break;
case 0x70: /*** ADE ***/
	AC = _SCMPBinaryAdd(AC,EX);
	CYCLES = CYCLES + 7;
	break;
case 0x78: /*** CAE ***/
	AC = _SCMPBinaryAdd(AC,EX ^ 0xFF);
	CYCLES = CYCLES + 8;
	break;
case 0x8f: /*** DLY $N ***/
	SCMPSetDelayCycles(OPERAND,AC);
	CYCLES = CYCLES + 13;
	break;
case 0x90: /*** JMP $O ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	P0 = EA;
	CYCLES = CYCLES + 11;
	break;
case 0x91: /*** JMP $N(P1) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	P0 = EA;
	CYCLES = CYCLES + 11;
	break;
case 0x92: /*** JMP $N(P2) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	P0 = EA;
	CYCLES = CYCLES + 11;
	break;
case 0x93: /*** JMP $N(P3) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	P0 = EA;
	CYCLES = CYCLES + 11;
	break;
case 0x94: /*** JP $O ***/
	if ((AC & 0x80) == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x95: /*** JP $N(P1) ***/
	if ((AC & 0x80) == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x96: /*** JP $N(P2) ***/
	if ((AC & 0x80) == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x97: /*** JP $N(P3) ***/
	if ((AC & 0x80) == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x98: /*** JZ $O ***/
	if (AC == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x99: /*** JZ $N(P1) ***/
	if (AC == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9a: /*** JZ $N(P2) ***/
	if (AC == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9b: /*** JZ $N(P3) ***/
	if (AC == 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9c: /*** JNZ $O ***/
	if (AC != 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9d: /*** JNZ $N(P1) ***/
	if (AC != 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9e: /*** JNZ $N(P2) ***/
	if (AC != 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0x9f: /*** JNZ $N(P3) ***/
	if (AC != 0) { EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	P0 = EA;
	 };
	CYCLES = CYCLES + 10;
	break;
case 0xa8: /*** ILD $O ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = (READ(EA)+1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xa9: /*** ILD $N(P1) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = (READ(EA)+1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xaa: /*** ILD $N(P2) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = (READ(EA)+1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xab: /*** ILD $N(P3) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = (READ(EA)+1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xb8: /*** DLD $O ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = (READ(EA)-1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xb9: /*** DLD $N(P1) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = (READ(EA)-1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xba: /*** DLD $N(P2) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = (READ(EA)-1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xbb: /*** DLD $N(P3) ***/
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = (READ(EA)-1) & 0xFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 22;
	break;
case 0xc0: /*** LD $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xc1: /*** LD $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xc2: /*** LD $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xc3: /*** LD $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xc4: /*** LDI $N ***/
	AC = OPERAND;
	CYCLES = CYCLES + 10;
	break;
case 0xc5: /*** LD @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = READ(EA);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xc6: /*** LD @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = READ(EA);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xc7: /*** LD @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = READ(EA);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xc8: /*** ST $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 18;
	break;
case 0xc9: /*** ST $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 18;
	break;
case 0xca: /*** ST $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 18;
	break;
case 0xcb: /*** ST $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	WRITE(EA,AC);
	CYCLES = CYCLES + 18;
	break;
case 0xcd: /*** ST @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	WRITE(EA,AC);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xce: /*** ST @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	WRITE(EA,AC);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xcf: /*** ST @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	WRITE(EA,AC);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xd0: /*** AND $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = AC & READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xd1: /*** AND $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC & READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xd2: /*** AND $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC & READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xd3: /*** AND $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC & READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xd4: /*** ANI $N ***/
	AC = AC & OPERAND;
	CYCLES = CYCLES + 10;
	break;
case 0xd5: /*** AND @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC & READ(EA);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xd6: /*** AND @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC & READ(EA);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xd7: /*** AND @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC & READ(EA);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xd8: /*** OR $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = AC | READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xd9: /*** OR $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC | READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xda: /*** OR $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC | READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xdb: /*** OR $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC | READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xdc: /*** ORI $N ***/
	AC = AC | OPERAND;
	CYCLES = CYCLES + 10;
	break;
case 0xdd: /*** OR @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC | READ(EA);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xde: /*** OR @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC | READ(EA);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xdf: /*** OR @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC | READ(EA);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xe0: /*** XOR $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = AC ^ READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xe1: /*** XOR $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC ^ READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xe2: /*** XOR $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC ^ READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xe3: /*** XOR $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC ^ READ(EA);
	CYCLES = CYCLES + 18;
	break;
case 0xe4: /*** XRI $N ***/
	AC = AC ^ OPERAND;
	CYCLES = CYCLES + 10;
	break;
case 0xe5: /*** XOR @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = AC ^ READ(EA);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xe6: /*** XOR @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = AC ^ READ(EA);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xe7: /*** XOR @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = AC ^ READ(EA);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xe8: /*** DAD $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = SCMPDecimalAdd(AC,READ(EA));
	CYCLES = CYCLES + 23;
	break;
case 0xe9: /*** DAD $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = SCMPDecimalAdd(AC,READ(EA));
	CYCLES = CYCLES + 23;
	break;
case 0xea: /*** DAD $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = SCMPDecimalAdd(AC,READ(EA));
	CYCLES = CYCLES + 23;
	break;
case 0xeb: /*** DAD $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = SCMPDecimalAdd(AC,READ(EA));
	CYCLES = CYCLES + 23;
	break;
case 0xec: /*** DAI $N ***/
	AC = _SCMPDecimalAdd(AC,OPERAND);
	CYCLES = CYCLES + 15;
	break;
case 0xed: /*** DAD @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = _SCMPDecimalAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 23;
	break;
case 0xee: /*** DAD @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = _SCMPDecimalAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 23;
	break;
case 0xef: /*** DAD @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = _SCMPDecimalAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 23;
	break;
case 0xf0: /*** ADD $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA));
	CYCLES = CYCLES + 18;
	break;
case 0xf1: /*** ADD $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA));
	CYCLES = CYCLES + 18;
	break;
case 0xf2: /*** ADD $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA));
	CYCLES = CYCLES + 18;
	break;
case 0xf3: /*** ADD $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA));
	CYCLES = CYCLES + 18;
	break;
case 0xf4: /*** ADI $N ***/
	AC = _SCMPBinaryAdd(AC,OPERAND);
	CYCLES = CYCLES + 11;
	break;
case 0xf5: /*** ADD @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xf6: /*** ADD @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xf7: /*** ADD @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA));
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 18;
	break;
case 0xf8: /*** CAD $O ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P0) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	CYCLES = CYCLES + 20;
	break;
case 0xf9: /*** CAD $N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	CYCLES = CYCLES + 20;
	break;
case 0xfa: /*** CAD $N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	CYCLES = CYCLES + 20;
	break;
case 0xfb: /*** CAD $N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	CYCLES = CYCLES + 20;
	break;
case 0xfc: /*** CAI $N ***/
	AC = _SCMPBinaryAdd(AC,OPERAND ^ 0xFF);
	CYCLES = CYCLES + 12;
	break;
case 0xfd: /*** CAD @$N(P1) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P1 = (P1 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P1) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	if ((OPERAND & 0x80) == 0) P1 = P1 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 20;
	break;
case 0xfe: /*** CAD @$N(P2) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P2 = (P2 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P2) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	if ((OPERAND & 0x80) == 0) P2 = P2 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 20;
	break;
case 0xff: /*** CAD @$N(P3) ***/
	if (OPERAND == 0x80) OPERAND = EX;
	if ((OPERAND & 0x80) != 0) P3 = (P3 - 256 + OPERAND) & 0xFFFF;
	EA = ((OPERAND & 0x80) != 0) ? (OPERAND | 0xFF00) : OPERAND;
	EA = (EA + P3) & 0xFFFF;
	AC = _SCMPBinaryAdd(AC,READ(EA) ^ 0xFF);
	if ((OPERAND & 0x80) == 0) P3 = P3 + OPERAND) & 0xFFFF;
	CYCLES = CYCLES + 20;
	break;
