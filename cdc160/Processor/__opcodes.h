case 000: /* err */
	haltFlag = 1;CYCLES(1);break;
case 001: /* sha */
	SHIFTINSTRUCTION();P = (P + 1) & 07777;CYCLES(1);break;
case 002: /* lpn */
	A &= (Z & 077);P = (P + 1) & 07777;CYCLES(1);break;
case 003: /* lsn */
	A ^= (Z & 077);P = (P + 1) & 07777;CYCLES(1);break;
case 004: /* ldn */
	A = (Z & 077);P = (P + 1) & 07777;CYCLES(1);break;
case 005: /* lcn */
	A = (Z & 077) ^ 07777;P = (P + 1) & 07777;CYCLES(1);break;
case 006: /* adn */
	ADD(Z & 077);P = (P + 1) & 07777;CYCLES(1);break;
case 007: /* sbn */
	ADD((Z & 077) ^ 07777);P = (P + 1) & 07777;CYCLES(1);break;
case 010: /* lpd */
	S = Z & 077;READ();A &= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 011: /* lpi */
	S = Z & 077;READ();S = Z;READ();A &= Z;P = (P + 1) & 07777;CYCLES(3);break;
case 012: /* lpf */
	S = ((P + (Z & 077)) & 07777);READ();A &= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 013: /* lpb */
	S = ((P - (Z & 077)) & 07777);READ();A &= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 014: /* lsd */
	S = Z & 077;READ();A ^= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 015: /* lsi */
	S = Z & 077;READ();S = Z;READ();A ^= Z;P = (P + 1) & 07777;CYCLES(3);break;
case 016: /* lsf */
	S = ((P + (Z & 077)) & 07777);READ();A ^= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 017: /* lsb */
	S = ((P - (Z & 077)) & 07777);READ();A ^= Z;P = (P + 1) & 07777;CYCLES(2);break;
case 020: /* ldd */
	S = Z & 077;READ();A = Z;P = (P + 1) & 07777;CYCLES(2);break;
case 021: /* ldi */
	S = Z & 077;READ();S = Z;READ();A = Z;P = (P + 1) & 07777;CYCLES(3);break;
case 022: /* ldf */
	S = ((P + (Z & 077)) & 07777);READ();A = Z;P = (P + 1) & 07777;CYCLES(2);break;
case 023: /* ldb */
	S = ((P - (Z & 077)) & 07777);READ();A = Z;P = (P + 1) & 07777;CYCLES(2);break;
case 024: /* lcd */
	S = Z & 077;READ();A = Z ^ 07777;P = (P + 1) & 07777;CYCLES(2);break;
case 025: /* lci */
	S = Z & 077;READ();S = Z;READ();A = Z ^ 07777;P = (P + 1) & 07777;CYCLES(3);break;
case 026: /* lcf */
	S = ((P + (Z & 077)) & 07777);READ();A = Z ^ 07777;P = (P + 1) & 07777;CYCLES(2);break;
case 027: /* lcb */
	S = ((P - (Z & 077)) & 07777);READ();A = Z ^ 07777;P = (P + 1) & 07777;CYCLES(2);break;
case 030: /* add */
	S = Z & 077;READ();ADD(Z);P = (P + 1) & 07777;CYCLES(2);break;
case 031: /* adi */
	S = Z & 077;READ();S = Z;READ();ADD(Z);P = (P + 1) & 07777;CYCLES(3);break;
case 032: /* adf */
	S = ((P + (Z & 077)) & 07777);READ();ADD(Z);P = (P + 1) & 07777;CYCLES(2);break;
case 033: /* adb */
	S = ((P - (Z & 077)) & 07777);READ();ADD(Z);P = (P + 1) & 07777;CYCLES(2);break;
case 034: /* sbd */
	S = Z & 077;READ();ADD(Z ^ 07777);P = (P + 1) & 07777;CYCLES(2);break;
case 035: /* sbi */
	S = Z & 077;READ();S = Z;READ();ADD(Z ^ 07777);P = (P + 1) & 07777;CYCLES(3);break;
case 036: /* sbf */
	S = ((P + (Z & 077)) & 07777);READ();ADD(Z ^ 07777);P = (P + 1) & 07777;CYCLES(2);break;
case 037: /* sbb */
	S = ((P - (Z & 077)) & 07777);READ();ADD(Z ^ 07777);P = (P + 1) & 07777;CYCLES(2);break;
case 040: /* std */
	S = Z & 077;Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 041: /* sti */
	S = Z & 077;READ();S = Z;Z = A;WRITE();P = (P + 1) & 07777;CYCLES(4);break;
case 042: /* stf */
	S = ((P + (Z & 077)) & 07777);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 043: /* stb */
	S = ((P - (Z & 077)) & 07777);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 044: /* srd */
	S = Z & 077;READ();A = Z = (ROTATE1(Z));WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 045: /* sri */
	S = Z & 077;READ();S = Z;READ();A = Z = (ROTATE1(Z));WRITE();P = (P + 1) & 07777;CYCLES(4);break;
case 046: /* srf */
	S = ((P + (Z & 077)) & 07777);READ();A = Z = (ROTATE1(Z));WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 047: /* srb */
	S = ((P - (Z & 077)) & 07777);READ();A = Z = (ROTATE1(Z));WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 050: /* rad */
	S = Z & 077;READ();ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 051: /* rai */
	S = Z & 077;READ();S = Z;READ();ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(4);break;
case 052: /* raf */
	S = ((P + (Z & 077)) & 07777);READ();ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 053: /* rab */
	S = ((P - (Z & 077)) & 07777);READ();ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 054: /* aod */
	S = Z & 077;READ();A = 1;ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 055: /* aoi */
	S = Z & 077;READ();S = Z;READ();A = 1;ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(4);break;
case 056: /* aof */
	S = ((P + (Z & 077)) & 07777);READ();A = 1;ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 057: /* aob */
	S = ((P - (Z & 077)) & 07777);READ();A = 1;ADD(Z);Z = A;WRITE();P = (P + 1) & 07777;CYCLES(3);break;
case 060: /* zjf */
	if (A == 0) { S = ((P + (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 061: /* nzf */
	if (A != 0) { S = ((P + (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 062: /* pjf */
	if ((A & 04000) == 0) { S = ((P + (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 063: /* njf */
	if ((A & 04000) != 0) { S = ((P + (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 064: /* zjb */
	if (A == 0) { S = ((P - (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 065: /* nzb */
	if (A != 0) { S = ((P - (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 066: /* pjb */
	if ((A & 04000) == 0) { S = ((P - (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 067: /* njb */
	if ((A & 04000) != 0) { S = ((P - (Z & 077)) & 07777);P = S; } else P = (P + 1) & 07777;CYCLES(1);break;
case 070: /* jpi */
	S = Z & 077;READ();S = Z; P = S;P = (P + 1) & 07777;CYCLES(2);break;
case 071: /* jfi */
	S = ((P + (Z & 077)) & 07777);READ();P = Z;P = (P + 1) & 07777;CYCLES(2);break;
case 072: /* inp */
	BLOCKTRANSFER('I');P = (P + 1) & 07777;CYCLES(2);break;
case 073: /* out */
	BLOCKTRANSFER('O');P = (P + 1) & 07777;CYCLES(2);break;
case 074: /* otn */
	Z = Z & 077;IOTRANSFER('O');P = (P + 1) & 07777;CYCLES(1);break;
case 075: /* exf */
	S = ((P + (Z & 077)) & 07777);READ();IOFUNCTIONCODE();P = (P + 1) & 07777;CYCLES(2);break;
case 076: /* ina */
	IOTRANSFER('I');A = Z;P = (P + 1) & 07777;CYCLES(1);break;
case 077: /* hlt */
	haltFlag = 1;CYCLES(1);break;
