case 0x00: // **** halt ****
    ;CYCLES(8);break;
case 0x01: // **** xae ****
    MB = EX;EX = AC;AC = MB;CYCLES(7);break;
case 0x02: // **** ccl ****
    SR = SR & 0x7F;CYCLES(5);break;
case 0x03: // **** scl ****
    SR = SR | 0x80;CYCLES(5);break;
case 0x04: // **** dint ****
    SR = SR & 0xF7;CYCLES(6);break;
case 0x05: // **** ien ****
    SR = SR | 0x08;CYCLES(0);break;
case 0x06: // **** csa ****
    AC = STATUSREG();CYCLES(5);break;
case 0x07: // **** cas ****
    SR = AC;CYCLES(0);break;
case 0x08: // **** nop ****
    ;CYCLES(5);break;
case 0x19: // **** sio ****
    EX = (EX >> 1) & 0x7F;CYCLES(5);break;
case 0x1c: // **** sr ****
    AC = (AC >> 1) & 0x7F;CYCLES(5);break;
case 0x1d: // **** srl ****
    AC = ((AC >> 1) & 0x7F) | (SR & 0x80);CYCLES(5);break;
case 0x1e: // **** rr ****
    AC = ((AC >> 1) & 0x7F) | ((AC & 1) << 7);CYCLES(5);break;
case 0x1f: // **** rrl ****
    operand = AC & 1;AC = ((AC >> 1) & 0x7F) | (SR & 0x80);SR = (SR & 0x7F) | (operand << 7);CYCLES(5);break;
case 0x30: // **** xpal p0 ****
    MB = (P0 & 0xFF);P0 = (P0 & 0xFF00) | AC;AC = MB;CYCLES(8);break;
case 0x31: // **** xpal p1 ****
    MB = (P1 & 0xFF);P1 = (P1 & 0xFF00) | AC;AC = MB;CYCLES(8);break;
case 0x32: // **** xpal p2 ****
    MB = (P2 & 0xFF);P2 = (P2 & 0xFF00) | AC;AC = MB;CYCLES(8);break;
case 0x33: // **** xpal p3 ****
    MB = (P3 & 0xFF);P3 = (P3 & 0xFF00) | AC;AC = MB;CYCLES(8);break;
case 0x34: // **** xpah p0 ****
    MB = (P0 >> 8) & 0xFF;P0 = (P0 & 0xFF) | (AC << 8);AC = MB;CYCLES(8);break;
case 0x35: // **** xpah p1 ****
    MB = (P1 >> 8) & 0xFF;P1 = (P1 & 0xFF) | (AC << 8);AC = MB;CYCLES(8);break;
case 0x36: // **** xpah p2 ****
    MB = (P2 >> 8) & 0xFF;P2 = (P2 & 0xFF) | (AC << 8);AC = MB;CYCLES(8);break;
case 0x37: // **** xpah p3 ****
    MB = (P3 >> 8) & 0xFF;P3 = (P3 & 0xFF) | (AC << 8);AC = MB;CYCLES(8);break;
case 0x3c: // **** xppc p0 ****
    ;CYCLES(7);break;
case 0x3d: // **** xppc p1 ****
    temp16 = P1;P1 = P0;P0 = temp16;CYCLES(7);break;
case 0x3e: // **** xppc p2 ****
    temp16 = P2;P2 = P0;P0 = temp16;CYCLES(7);break;
case 0x3f: // **** xppc p3 ****
    temp16 = P3;P3 = P0;P0 = temp16;CYCLES(7);break;
case 0x40: // **** lde ****
    AC = EX;CYCLES(6);break;
case 0x50: // **** ane ****
    AC = AC & EX;CYCLES(6);break;
case 0x58: // **** ore ****
    AC = AC | EX;CYCLES(6);break;
case 0x60: // **** xre ****
    AC = AC ^ EX;CYCLES(6);break;
case 0x68: // **** dae ****
    MB = EX;DECIMALADD();CYCLES(11);break;
case 0x70: // **** ade ****
    MB = EX;BINARYADD();CYCLES(7);break;
case 0x78: // **** cae ****
    MB = EX ^ 0xFF;BINARYADD();CYCLES(8);break;
case 0x8f: // **** dly ## ****
    DELAYCPU(operand,AC);AC = 0;CYCLES(13);break;
case 0x90: // **** jmp ##(pc) ****
    EAC(P0);P0 = MA;CYCLES(11);break;
case 0x91: // **** jmp ##(p1) ****
    EAC(P1);P0 = MA;CYCLES(11);break;
case 0x92: // **** jmp ##(p2) ****
    EAC(P2);P0 = MA;CYCLES(11);break;
case 0x93: // **** jmp ##(p3) ****
    EAC(P3);P0 = MA;CYCLES(11);break;
case 0x94: // **** jp ##(pc) ****
    EAC(P0);if ((AC & 0x80) == 0) P0 = MA;CYCLES(10);break;
case 0x95: // **** jp ##(p1) ****
    EAC(P1);if ((AC & 0x80) == 0) P0 = MA;CYCLES(10);break;
case 0x96: // **** jp ##(p2) ****
    EAC(P2);if ((AC & 0x80) == 0) P0 = MA;CYCLES(10);break;
case 0x97: // **** jp ##(p3) ****
    EAC(P3);if ((AC & 0x80) == 0) P0 = MA;CYCLES(10);break;
case 0x98: // **** jz ##(pc) ****
    EAC(P0);if (AC == 0) P0 = MA;CYCLES(10);break;
case 0x99: // **** jz ##(p1) ****
    EAC(P1);if (AC == 0) P0 = MA;CYCLES(10);break;
case 0x9a: // **** jz ##(p2) ****
    EAC(P2);if (AC == 0) P0 = MA;CYCLES(10);break;
case 0x9b: // **** jz ##(p3) ****
    EAC(P3);if (AC == 0) P0 = MA;CYCLES(10);break;
case 0x9c: // **** jnz ##(pc) ****
    EAC(P0);if (AC != 0) P0 = MA;CYCLES(10);break;
case 0x9d: // **** jnz ##(p1) ****
    EAC(P1);if (AC != 0) P0 = MA;CYCLES(10);break;
case 0x9e: // **** jnz ##(p2) ****
    EAC(P2);if (AC != 0) P0 = MA;CYCLES(10);break;
case 0x9f: // **** jnz ##(p3) ****
    EAC(P3);if (AC != 0) P0 = MA;CYCLES(10);break;
case 0xa8: // **** ild ##(pc) ****
    EAC(P0);READ();AC = MB = (MB + 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xa9: // **** ild ##(p1) ****
    EAC(P1);READ();AC = MB = (MB + 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xaa: // **** ild ##(p2) ****
    EAC(P2);READ();AC = MB = (MB + 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xab: // **** ild ##(p3) ****
    EAC(P3);READ();AC = MB = (MB + 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xb8: // **** dld ##(pc) ****
    EAC(P0);READ();AC = MB = (MB - 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xb9: // **** dld ##(p1) ****
    EAC(P1);READ();AC = MB = (MB - 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xba: // **** dld ##(p2) ****
    EAC(P2);READ();AC = MB = (MB - 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xbb: // **** dld ##(p3) ****
    EAC(P3);READ();AC = MB = (MB - 1) & 0xFF;WRITE();CYCLES(22);break;
case 0xc0: // **** ld ##(pc) ****
    CHECKEX();EAC(P0);READ();AC = MB;CYCLES(18);break;
case 0xc1: // **** ld ##(p1) ****
    CHECKEX();EAC(P1);READ();AC = MB;CYCLES(18);break;
case 0xc2: // **** ld ##(p2) ****
    CHECKEX();EAC(P2);READ();AC = MB;CYCLES(18);break;
case 0xc3: // **** ld ##(p3) ****
    CHECKEX();EAC(P3);READ();AC = MB;CYCLES(18);break;
case 0xc4: // **** ldi ## ****
    AC = operand;CYCLES(10);break;
case 0xc5: // **** ld @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();AC = MB;CYCLES(18);break;
case 0xc6: // **** ld @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();AC = MB;CYCLES(18);break;
case 0xc7: // **** ld @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();AC = MB;CYCLES(18);break;
case 0xc8: // **** st ##(pc) ****
    CHECKEX();EAC(P0);READ();MB = AC;WRITE();CYCLES(18);break;
case 0xc9: // **** st ##(p1) ****
    CHECKEX();EAC(P1);READ();MB = AC;WRITE();CYCLES(18);break;
case 0xca: // **** st ##(p2) ****
    CHECKEX();EAC(P2);READ();MB = AC;WRITE();CYCLES(18);break;
case 0xcb: // **** st ##(p3) ****
    CHECKEX();EAC(P3);READ();MB = AC;WRITE();CYCLES(18);break;
case 0xcd: // **** st @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);MB = AC;WRITE();CYCLES(18);break;
case 0xce: // **** st @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);MB = AC;WRITE();CYCLES(18);break;
case 0xcf: // **** st @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);MB = AC;WRITE();CYCLES(18);break;
case 0xd0: // **** and ##(pc) ****
    CHECKEX();EAC(P0);READ();AC = AC & MB;CYCLES(18);break;
case 0xd1: // **** and ##(p1) ****
    CHECKEX();EAC(P1);READ();AC = AC & MB;CYCLES(18);break;
case 0xd2: // **** and ##(p2) ****
    CHECKEX();EAC(P2);READ();AC = AC & MB;CYCLES(18);break;
case 0xd3: // **** and ##(p3) ****
    CHECKEX();EAC(P3);READ();AC = AC & MB;CYCLES(18);break;
case 0xd4: // **** ani ## ****
    AC = AC & operand;CYCLES(10);break;
case 0xd5: // **** and @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();AC = AC & MB;CYCLES(18);break;
case 0xd6: // **** and @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();AC = AC & MB;CYCLES(18);break;
case 0xd7: // **** and @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();AC = AC & MB;CYCLES(18);break;
case 0xd8: // **** or ##(pc) ****
    CHECKEX();EAC(P0);READ();AC = AC | MB;CYCLES(18);break;
case 0xd9: // **** or ##(p1) ****
    CHECKEX();EAC(P1);READ();AC = AC | MB;CYCLES(18);break;
case 0xda: // **** or ##(p2) ****
    CHECKEX();EAC(P2);READ();AC = AC | MB;CYCLES(18);break;
case 0xdb: // **** or ##(p3) ****
    CHECKEX();EAC(P3);READ();AC = AC | MB;CYCLES(18);break;
case 0xdc: // **** ori ## ****
    AC = AC | operand;CYCLES(10);break;
case 0xdd: // **** or @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();AC = AC | MB;CYCLES(18);break;
case 0xde: // **** or @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();AC = AC | MB;CYCLES(18);break;
case 0xdf: // **** or @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();AC = AC | MB;CYCLES(18);break;
case 0xe0: // **** xor ##(pc) ****
    CHECKEX();EAC(P0);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe1: // **** xor ##(p1) ****
    CHECKEX();EAC(P1);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe2: // **** xor ##(p2) ****
    CHECKEX();EAC(P2);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe3: // **** xor ##(p3) ****
    CHECKEX();EAC(P3);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe4: // **** xri ## ****
    AC = AC ^ operand;CYCLES(10);break;
case 0xe5: // **** xor @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe6: // **** xor @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe7: // **** xor @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();AC = AC ^ MB;CYCLES(18);break;
case 0xe8: // **** dad ##(pc) ****
    CHECKEX();EAC(P0);READ();DECIMALADD();CYCLES(23);break;
case 0xe9: // **** dad ##(p1) ****
    CHECKEX();EAC(P1);READ();DECIMALADD();CYCLES(23);break;
case 0xea: // **** dad ##(p2) ****
    CHECKEX();EAC(P2);READ();DECIMALADD();CYCLES(23);break;
case 0xeb: // **** dad ##(p3) ****
    CHECKEX();EAC(P3);READ();DECIMALADD();CYCLES(23);break;
case 0xec: // **** dai ## ****
    MB = operand;DECIMALADD();CYCLES(15);break;
case 0xed: // **** dad @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();DECIMALADD();CYCLES(23);break;
case 0xee: // **** dad @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();DECIMALADD();CYCLES(23);break;
case 0xef: // **** dad @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();DECIMALADD();CYCLES(23);break;
case 0xf0: // **** add ##(pc) ****
    CHECKEX();EAC(P0);READ();BINARYADD();CYCLES(19);break;
case 0xf1: // **** add ##(p1) ****
    CHECKEX();EAC(P1);READ();BINARYADD();CYCLES(19);break;
case 0xf2: // **** add ##(p2) ****
    CHECKEX();EAC(P2);READ();BINARYADD();CYCLES(19);break;
case 0xf3: // **** add ##(p3) ****
    CHECKEX();EAC(P3);READ();BINARYADD();CYCLES(19);break;
case 0xf4: // **** adi ## ****
    MB = operand;BINARYADD();CYCLES(11);break;
case 0xf5: // **** add @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();BINARYADD();CYCLES(19);break;
case 0xf6: // **** add @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();BINARYADD();CYCLES(19);break;
case 0xf7: // **** add @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();BINARYADD();CYCLES(19);break;
case 0xf8: // **** cad ##(pc) ****
    CHECKEX();EAC(P0);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xf9: // **** cad ##(p1) ****
    CHECKEX();EAC(P1);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xfa: // **** cad ##(p2) ****
    CHECKEX();EAC(P2);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xfb: // **** cad ##(p3) ****
    CHECKEX();EAC(P3);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xfc: // **** cai ## ****
    MB = operand;MB = MB ^ 0xFF;BINARYADD();CYCLES(12);break;
case 0xfd: // **** cad @##(p1) ****
    CHECKEX();PREINDEX(P1);MA = P1;POSTINDEX(P1);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xfe: // **** cad @##(p2) ****
    CHECKEX();PREINDEX(P2);MA = P2;POSTINDEX(P2);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
case 0xff: // **** cad @##(p3) ****
    CHECKEX();PREINDEX(P3);MA = P3;POSTINDEX(P3);READ();MB = MB ^ 0xFF;BINARYADD();CYCLES(20);break;
