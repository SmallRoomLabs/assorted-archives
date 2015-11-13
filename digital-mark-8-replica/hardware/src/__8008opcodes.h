case 0x00: // hlt
    HaltFlag = 1;CYCLES(4);break;
case 0x01: // hlt
    HaltFlag = 1;CYCLES(4);break;
case 0x02: // rlc
    Carry = (A >> 7);A = ((A << 1) | Carry) & 0xFF;CYCLES(5);break;
case 0x03: // rnc
    if (Carry == 0) POP();CYCLES(5);break;
case 0x04: // adi @1
    FETCH();Carry = 0;A = PSZValue = _Add(MB);CYCLES(8);break;
case 0x05: // rst 00
    PUSH();PCTR = 0x00;CYCLES(5);break;
case 0x06: // mvi a,@1
    FETCH();A = MB;CYCLES(8);break;
case 0x07: // ret
    POP();CYCLES(5);break;
case 0x08: // inr b
    B = PSZValue = (B + 1) & 0xFF;CYCLES(5);break;
case 0x09: // dcr b
    B = PSZValue = (B - 1) & 0xFF;CYCLES(5);break;
case 0x0a: // rrc
    Carry = A & 1;A = (A >> 1) | (Carry << 7);CYCLES(5);break;
case 0x0b: // rnz
    if (PSZValue != 0) POP();CYCLES(5);break;
case 0x0c: // aci @1
    FETCH();A = PSZValue = _Add(MB);CYCLES(8);break;
case 0x0d: // rst 08
    PUSH();PCTR = 0x08;CYCLES(5);break;
case 0x0e: // mvi b,@1
    FETCH();B = MB;CYCLES(8);break;
case 0x0f: // ret
    POP();CYCLES(5);break;
case 0x10: // inr c
    C = PSZValue = (C + 1) & 0xFF;CYCLES(5);break;
case 0x11: // dcr c
    C = PSZValue = (C - 1) & 0xFF;CYCLES(5);break;
case 0x12: // ral
    temp16 = (A << 1) | Carry;A = temp16 & 0xFF;Carry = temp16 >> 8;CYCLES(5);break;
case 0x13: // rp
    if ((PSZValue & 0x80) == 0) POP();CYCLES(5);break;
case 0x14: // sui @1
    FETCH();Carry = 0;A = PSZValue = _Sub(MB);CYCLES(8);break;
case 0x15: // rst 10
    PUSH();PCTR = 0x10;CYCLES(5);break;
case 0x16: // mvi c,@1
    FETCH();C = MB;CYCLES(8);break;
case 0x17: // ret
    POP();CYCLES(5);break;
case 0x18: // inr d
    D = PSZValue = (D + 1) & 0xFF;CYCLES(5);break;
case 0x19: // dcr d
    D = PSZValue = (D - 1) & 0xFF;CYCLES(5);break;
case 0x1a: // rar
    temp16 = A | (Carry << 8);A = temp16 >> 1;Carry = temp16 & 1;CYCLES(5);break;
case 0x1b: // rpo
    if (_Parity(PSZValue) != 0) POP();CYCLES(5);break;
case 0x1c: // sbi @1
    FETCH();A = PSZValue = _Sub(MB);CYCLES(8);break;
case 0x1d: // rst 18
    PUSH();PCTR = 0x18;CYCLES(5);break;
case 0x1e: // mvi d,@1
    FETCH();D = MB;CYCLES(8);break;
case 0x1f: // ret
    POP();CYCLES(5);break;
case 0x20: // inr e
    E = PSZValue = (E + 1) & 0xFF;CYCLES(5);break;
case 0x21: // dcr e
    E = PSZValue = (E - 1) & 0xFF;CYCLES(5);break;
case 0x23: // rc
    if (Carry != 0) POP();CYCLES(5);break;
case 0x24: // ani @1
    FETCH();Carry = 0;A = PSZValue = A & MB;CYCLES(8);break;
case 0x25: // rst 20
    PUSH();PCTR = 0x20;CYCLES(5);break;
case 0x26: // mvi e,@1
    FETCH();E = MB;CYCLES(8);break;
case 0x27: // ret
    POP();CYCLES(5);break;
case 0x28: // inr h
    H = PSZValue = (H + 1) & 0xFF;CYCLES(5);break;
case 0x29: // dcr h
    H = PSZValue = (H - 1) & 0xFF;CYCLES(5);break;
case 0x2b: // rz
    if (PSZValue == 0) POP();CYCLES(5);break;
case 0x2c: // xri @1
    FETCH();Carry = 0;A = PSZValue = A ^ MB;CYCLES(8);break;
case 0x2d: // rst 28
    PUSH();PCTR = 0x28;CYCLES(5);break;
case 0x2e: // mvi h,@1
    FETCH();H = MB;CYCLES(8);break;
case 0x2f: // ret
    POP();CYCLES(5);break;
case 0x30: // inr l
    L = PSZValue = (L + 1) & 0xFF;CYCLES(5);break;
case 0x31: // dcr l
    L = PSZValue = (L - 1) & 0xFF;CYCLES(5);break;
case 0x33: // rm
    if ((PSZValue & 0x80) != 0) POP();CYCLES(5);break;
case 0x34: // ori @1
    FETCH();Carry = 0;A = PSZValue = A | MB;CYCLES(8);break;
case 0x35: // rst 30
    PUSH();PCTR = 0x30;CYCLES(5);break;
case 0x36: // mvi l,@1
    FETCH();L = MB;CYCLES(8);break;
case 0x37: // ret
    POP();CYCLES(5);break;
case 0x3b: // rpe
    if (_Parity(PSZValue) == 0) POP();CYCLES(5);break;
case 0x3c: // cpi @1
    FETCH();Carry = 0;PSZValue = _Sub(MB);CYCLES(8);break;
case 0x3d: // rst 38
    PUSH();PCTR = 0x38;CYCLES(5);break;
case 0x3e: // mvi m,@1
    FETCH();HL();WRITE();CYCLES(9);break;
case 0x3f: // ret
    POP();CYCLES(5);break;
case 0x40: // jnc @2
    FETCH16();if (Carry == 0) PCTR = temp16;CYCLES(10);break;
case 0x41: // in 00
    READPORT00();A = MB;CYCLES(8);break;
case 0x42: // cnc @2
    FETCH16();if (Carry == 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x43: // in 01
    READPORT01();A = MB;CYCLES(8);break;
case 0x44: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x45: // in 02
    READPORT02();A = MB;CYCLES(8);break;
case 0x46: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x47: // in 03
    READPORT03();A = MB;CYCLES(8);break;
case 0x48: // jnz @2
    FETCH16();if (PSZValue != 0) PCTR = temp16;CYCLES(10);break;
case 0x49: // in 04
    READPORT04();A = MB;CYCLES(8);break;
case 0x4a: // cnz @2
    FETCH16();if (PSZValue != 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x4b: // in 05
    READPORT05();A = MB;CYCLES(8);break;
case 0x4c: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x4d: // in 06
    READPORT06();A = MB;CYCLES(8);break;
case 0x4e: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x4f: // in 07
    READPORT07();A = MB;CYCLES(8);break;
case 0x50: // jp @2
    FETCH16();if ((PSZValue & 0x80) == 0) PCTR = temp16;CYCLES(10);break;
case 0x51: // out 08
    MB = A;WRITEPORT08();CYCLES(6);break;
case 0x52: // cp @2
    FETCH16();if ((PSZValue & 0x80) == 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x53: // out 09
    MB = A;WRITEPORT09();CYCLES(6);break;
case 0x54: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x55: // out 0a
    MB = A;WRITEPORT0A();CYCLES(6);break;
case 0x56: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x57: // out 0b
    MB = A;WRITEPORT0B();CYCLES(6);break;
case 0x58: // jpo @2
    FETCH16();if (_Parity(PSZValue) != 0) PCTR = temp16;CYCLES(10);break;
case 0x59: // out 0c
    MB = A;WRITEPORT0C();CYCLES(6);break;
case 0x5a: // cpo @2
    FETCH16();if (_Parity(PSZValue) != 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x5b: // out 0d
    MB = A;WRITEPORT0D();CYCLES(6);break;
case 0x5c: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x5d: // out 0e
    MB = A;WRITEPORT0E();CYCLES(6);break;
case 0x5e: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x5f: // out 0f
    MB = A;WRITEPORT0F();CYCLES(6);break;
case 0x60: // jc @2
    FETCH16();if (Carry != 0) PCTR = temp16;CYCLES(10);break;
case 0x61: // out 10
    MB = A;WRITEPORT10();CYCLES(6);break;
case 0x62: // cc @2
    FETCH16();if (Carry != 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x63: // out 11
    MB = A;WRITEPORT11();CYCLES(6);break;
case 0x64: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x65: // out 12
    MB = A;WRITEPORT12();CYCLES(6);break;
case 0x66: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x67: // out 13
    MB = A;WRITEPORT13();CYCLES(6);break;
case 0x68: // jz @2
    FETCH16();if (PSZValue == 0) PCTR = temp16;CYCLES(10);break;
case 0x69: // out 14
    MB = A;WRITEPORT14();CYCLES(6);break;
case 0x6a: // cz @2
    FETCH16();if (PSZValue == 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x6b: // out 15
    MB = A;WRITEPORT15();CYCLES(6);break;
case 0x6c: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x6d: // out 16
    MB = A;WRITEPORT16();CYCLES(6);break;
case 0x6e: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x6f: // out 17
    MB = A;WRITEPORT17();CYCLES(6);break;
case 0x70: // jm @2
    FETCH16();if ((PSZValue & 0x80) != 0) PCTR = temp16;CYCLES(10);break;
case 0x71: // out 18
    MB = A;WRITEPORT18();CYCLES(6);break;
case 0x72: // cm @2
    FETCH16();if ((PSZValue & 0x80) != 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x73: // out 19
    MB = A;WRITEPORT19();CYCLES(6);break;
case 0x74: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x75: // out 1a
    MB = A;WRITEPORT1A();CYCLES(6);break;
case 0x76: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x77: // out 1b
    MB = A;WRITEPORT1B();CYCLES(6);break;
case 0x78: // jpe @2
    FETCH16();if (_Parity(PSZValue) == 0) PCTR = temp16;CYCLES(10);break;
case 0x79: // out 1c
    MB = A;WRITEPORT1C();CYCLES(6);break;
case 0x7a: // cpe @2
    FETCH16();if (_Parity(PSZValue) == 0) {PUSH();PCTR=temp16;};CYCLES(10);break;
case 0x7b: // out 1d
    MB = A;WRITEPORT1D();CYCLES(6);break;
case 0x7c: // jmp @2
    FETCH16();PCTR = temp16;CYCLES(11);break;
case 0x7d: // out 1e
    MB = A;WRITEPORT1E();CYCLES(6);break;
case 0x7e: // call @2
    FETCH16();PUSH();PCTR = temp16;CYCLES(11);break;
case 0x7f: // out 1f
    MB = A;WRITEPORT1F();CYCLES(6);break;
case 0x80: // add a
    Carry = 0;A = PSZValue = _Add(A);CYCLES(5);break;
case 0x81: // add b
    Carry = 0;A = PSZValue = _Add(B);CYCLES(5);break;
case 0x82: // add c
    Carry = 0;A = PSZValue = _Add(C);CYCLES(5);break;
case 0x83: // add d
    Carry = 0;A = PSZValue = _Add(D);CYCLES(5);break;
case 0x84: // add e
    Carry = 0;A = PSZValue = _Add(E);CYCLES(5);break;
case 0x85: // add h
    Carry = 0;A = PSZValue = _Add(H);CYCLES(5);break;
case 0x86: // add l
    Carry = 0;A = PSZValue = _Add(L);CYCLES(5);break;
case 0x87: // add m
    Carry = 0;A = PSZValue = _Add(_ReadHL());CYCLES(8);break;
case 0x88: // adc a
    A = PSZValue = _Add(A);CYCLES(5);break;
case 0x89: // adc b
    A = PSZValue = _Add(B);CYCLES(5);break;
case 0x8a: // adc c
    A = PSZValue = _Add(C);CYCLES(5);break;
case 0x8b: // adc d
    A = PSZValue = _Add(D);CYCLES(5);break;
case 0x8c: // adc e
    A = PSZValue = _Add(E);CYCLES(5);break;
case 0x8d: // adc h
    A = PSZValue = _Add(H);CYCLES(5);break;
case 0x8e: // adc l
    A = PSZValue = _Add(L);CYCLES(5);break;
case 0x8f: // adc m
    A = PSZValue = _Add(_ReadHL());CYCLES(8);break;
case 0x90: // sub a
    Carry = 0;A = PSZValue = _Sub(A);CYCLES(5);break;
case 0x91: // sub b
    Carry = 0;A = PSZValue = _Sub(B);CYCLES(5);break;
case 0x92: // sub c
    Carry = 0;A = PSZValue = _Sub(C);CYCLES(5);break;
case 0x93: // sub d
    Carry = 0;A = PSZValue = _Sub(D);CYCLES(5);break;
case 0x94: // sub e
    Carry = 0;A = PSZValue = _Sub(E);CYCLES(5);break;
case 0x95: // sub h
    Carry = 0;A = PSZValue = _Sub(H);CYCLES(5);break;
case 0x96: // sub l
    Carry = 0;A = PSZValue = _Sub(L);CYCLES(5);break;
case 0x97: // sub m
    Carry = 0;A = PSZValue = _Sub(_ReadHL());CYCLES(8);break;
case 0x98: // sbb a
    A = PSZValue = _Sub(A);CYCLES(5);break;
case 0x99: // sbb b
    A = PSZValue = _Sub(B);CYCLES(5);break;
case 0x9a: // sbb c
    A = PSZValue = _Sub(C);CYCLES(5);break;
case 0x9b: // sbb d
    A = PSZValue = _Sub(D);CYCLES(5);break;
case 0x9c: // sbb e
    A = PSZValue = _Sub(E);CYCLES(5);break;
case 0x9d: // sbb h
    A = PSZValue = _Sub(H);CYCLES(5);break;
case 0x9e: // sbb l
    A = PSZValue = _Sub(L);CYCLES(5);break;
case 0x9f: // sbb m
    A = PSZValue = _Sub(_ReadHL());CYCLES(8);break;
case 0xa0: // and a
    Carry = 0;A = PSZValue = A & A;CYCLES(5);break;
case 0xa1: // and b
    Carry = 0;A = PSZValue = A & B;CYCLES(5);break;
case 0xa2: // and c
    Carry = 0;A = PSZValue = A & C;CYCLES(5);break;
case 0xa3: // and d
    Carry = 0;A = PSZValue = A & D;CYCLES(5);break;
case 0xa4: // and e
    Carry = 0;A = PSZValue = A & E;CYCLES(5);break;
case 0xa5: // and h
    Carry = 0;A = PSZValue = A & H;CYCLES(5);break;
case 0xa6: // and l
    Carry = 0;A = PSZValue = A & L;CYCLES(5);break;
case 0xa7: // and m
    Carry = 0;A = PSZValue = A & _ReadHL();CYCLES(8);break;
case 0xa8: // xra a
    Carry = 0;A = PSZValue = A ^ A;CYCLES(5);break;
case 0xa9: // xra b
    Carry = 0;A = PSZValue = A ^ B;CYCLES(5);break;
case 0xaa: // xra c
    Carry = 0;A = PSZValue = A ^ C;CYCLES(5);break;
case 0xab: // xra d
    Carry = 0;A = PSZValue = A ^ D;CYCLES(5);break;
case 0xac: // xra e
    Carry = 0;A = PSZValue = A ^ E;CYCLES(5);break;
case 0xad: // xra h
    Carry = 0;A = PSZValue = A ^ H;CYCLES(5);break;
case 0xae: // xra l
    Carry = 0;A = PSZValue = A ^ L;CYCLES(5);break;
case 0xaf: // xra m
    Carry = 0;A = PSZValue = A ^ _ReadHL();CYCLES(8);break;
case 0xb0: // ora a
    Carry = 0;A = PSZValue = A | A;CYCLES(5);break;
case 0xb1: // ora b
    Carry = 0;A = PSZValue = A | B;CYCLES(5);break;
case 0xb2: // ora c
    Carry = 0;A = PSZValue = A | C;CYCLES(5);break;
case 0xb3: // ora d
    Carry = 0;A = PSZValue = A | D;CYCLES(5);break;
case 0xb4: // ora e
    Carry = 0;A = PSZValue = A | E;CYCLES(5);break;
case 0xb5: // ora h
    Carry = 0;A = PSZValue = A | H;CYCLES(5);break;
case 0xb6: // ora l
    Carry = 0;A = PSZValue = A | L;CYCLES(5);break;
case 0xb7: // ora m
    Carry = 0;A = PSZValue = A | _ReadHL();CYCLES(8);break;
case 0xb8: // cmp a
    Carry = 0;PSZValue = _Sub(A);CYCLES(5);break;
case 0xb9: // cmp b
    Carry = 0;PSZValue = _Sub(B);CYCLES(5);break;
case 0xba: // cmp c
    Carry = 0;PSZValue = _Sub(C);CYCLES(5);break;
case 0xbb: // cmp d
    Carry = 0;PSZValue = _Sub(D);CYCLES(5);break;
case 0xbc: // cmp e
    Carry = 0;PSZValue = _Sub(E);CYCLES(5);break;
case 0xbd: // cmp h
    Carry = 0;PSZValue = _Sub(H);CYCLES(5);break;
case 0xbe: // cmp l
    Carry = 0;PSZValue = _Sub(L);CYCLES(5);break;
case 0xbf: // cmp m
    Carry = 0;PSZValue = _Sub(_ReadHL());CYCLES(8);break;
case 0xc0: // mov a,a
    A = A;CYCLES(5);break;
case 0xc1: // mov a,b
    A = B;CYCLES(5);break;
case 0xc2: // mov a,c
    A = C;CYCLES(5);break;
case 0xc3: // mov a,d
    A = D;CYCLES(5);break;
case 0xc4: // mov a,e
    A = E;CYCLES(5);break;
case 0xc5: // mov a,h
    A = H;CYCLES(5);break;
case 0xc6: // mov a,l
    A = L;CYCLES(5);break;
case 0xc7: // mov a,m
    A = _ReadHL();CYCLES(8);break;
case 0xc8: // mov b,a
    B = A;CYCLES(5);break;
case 0xc9: // mov b,b
    B = B;CYCLES(5);break;
case 0xca: // mov b,c
    B = C;CYCLES(5);break;
case 0xcb: // mov b,d
    B = D;CYCLES(5);break;
case 0xcc: // mov b,e
    B = E;CYCLES(5);break;
case 0xcd: // mov b,h
    B = H;CYCLES(5);break;
case 0xce: // mov b,l
    B = L;CYCLES(5);break;
case 0xcf: // mov b,m
    B = _ReadHL();CYCLES(8);break;
case 0xd0: // mov c,a
    C = A;CYCLES(5);break;
case 0xd1: // mov c,b
    C = B;CYCLES(5);break;
case 0xd2: // mov c,c
    C = C;CYCLES(5);break;
case 0xd3: // mov c,d
    C = D;CYCLES(5);break;
case 0xd4: // mov c,e
    C = E;CYCLES(5);break;
case 0xd5: // mov c,h
    C = H;CYCLES(5);break;
case 0xd6: // mov c,l
    C = L;CYCLES(5);break;
case 0xd7: // mov c,m
    C = _ReadHL();CYCLES(8);break;
case 0xd8: // mov d,a
    D = A;CYCLES(5);break;
case 0xd9: // mov d,b
    D = B;CYCLES(5);break;
case 0xda: // mov d,c
    D = C;CYCLES(5);break;
case 0xdb: // mov d,d
    D = D;CYCLES(5);break;
case 0xdc: // mov d,e
    D = E;CYCLES(5);break;
case 0xdd: // mov d,h
    D = H;CYCLES(5);break;
case 0xde: // mov d,l
    D = L;CYCLES(5);break;
case 0xdf: // mov d,m
    D = _ReadHL();CYCLES(8);break;
case 0xe0: // mov e,a
    E = A;CYCLES(5);break;
case 0xe1: // mov e,b
    E = B;CYCLES(5);break;
case 0xe2: // mov e,c
    E = C;CYCLES(5);break;
case 0xe3: // mov e,d
    E = D;CYCLES(5);break;
case 0xe4: // mov e,e
    E = E;CYCLES(5);break;
case 0xe5: // mov e,h
    E = H;CYCLES(5);break;
case 0xe6: // mov e,l
    E = L;CYCLES(5);break;
case 0xe7: // mov e,m
    E = _ReadHL();CYCLES(8);break;
case 0xe8: // mov h,a
    H = A;CYCLES(5);break;
case 0xe9: // mov h,b
    H = B;CYCLES(5);break;
case 0xea: // mov h,c
    H = C;CYCLES(5);break;
case 0xeb: // mov h,d
    H = D;CYCLES(5);break;
case 0xec: // mov h,e
    H = E;CYCLES(5);break;
case 0xed: // mov h,h
    H = H;CYCLES(5);break;
case 0xee: // mov h,l
    H = L;CYCLES(5);break;
case 0xef: // mov h,m
    H = _ReadHL();CYCLES(8);break;
case 0xf0: // mov l,a
    L = A;CYCLES(5);break;
case 0xf1: // mov l,b
    L = B;CYCLES(5);break;
case 0xf2: // mov l,c
    L = C;CYCLES(5);break;
case 0xf3: // mov l,d
    L = D;CYCLES(5);break;
case 0xf4: // mov l,e
    L = E;CYCLES(5);break;
case 0xf5: // mov l,h
    L = H;CYCLES(5);break;
case 0xf6: // mov l,l
    L = L;CYCLES(5);break;
case 0xf7: // mov l,m
    L = _ReadHL();CYCLES(8);break;
case 0xf8: // mov m,a
    HL();MB = A;WRITE();CYCLES(7);break;
case 0xf9: // mov m,b
    HL();MB = B;WRITE();CYCLES(7);break;
case 0xfa: // mov m,c
    HL();MB = C;WRITE();CYCLES(7);break;
case 0xfb: // mov m,d
    HL();MB = D;WRITE();CYCLES(7);break;
case 0xfc: // mov m,e
    HL();MB = E;WRITE();CYCLES(7);break;
case 0xfd: // mov m,h
    HL();MB = H;WRITE();CYCLES(7);break;
case 0xfe: // mov m,l
    HL();MB = L;WRITE();CYCLES(7);break;
case 0xff: // hlt
    HaltFlag = 1;CYCLES(4);break;
