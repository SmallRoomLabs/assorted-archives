// **************** IDL ***************
case 0x00:
    ;;
    cycles += 2;
    break;
// **************** LDN R1 ***************
case 0x01:
case 0x02:
case 0x03:
case 0x04:
case 0x05:
case 0x06:
case 0x07:
case 0x08:
case 0x09:
case 0x0a:
case 0x0b:
case 0x0c:
case 0x0d:
case 0x0e:
case 0x0f:
    MA = *pReg;READ();D = MB;
    cycles += 2;
    break;
// **************** INC R0 ***************
case 0x10:
case 0x11:
case 0x12:
case 0x13:
case 0x14:
case 0x15:
case 0x16:
case 0x17:
case 0x18:
case 0x19:
case 0x1a:
case 0x1b:
case 0x1c:
case 0x1d:
case 0x1e:
case 0x1f:
    INCREG();
    cycles += 2;
    break;
// **************** DEC R0 ***************
case 0x20:
case 0x21:
case 0x22:
case 0x23:
case 0x24:
case 0x25:
case 0x26:
case 0x27:
case 0x28:
case 0x29:
case 0x2a:
case 0x2b:
case 0x2c:
case 0x2d:
case 0x2e:
case 0x2f:
    DECREG();
    cycles += 2;
    break;
// **************** BR @1 ***************
case 0x30:
    BRANCH(1);
    cycles += 2;
    break;
// **************** BQ @1 ***************
case 0x31:
    BRANCH(Q != 0);
    cycles += 2;
    break;
// **************** BZ @1 ***************
case 0x32:
    BRANCH(D == 0);
    cycles += 2;
    break;
// **************** BDF @1 ***************
case 0x33:
    BRANCH(DF != 0);
    cycles += 2;
    break;
// **************** B1 @1 ***************
case 0x34:
    BRANCH(EFLAG1() != 0);
    cycles += 2;
    break;
// **************** B2 @1 ***************
case 0x35:
    BRANCH(EFLAG2() != 0);
    cycles += 2;
    break;
// **************** B3 @1 ***************
case 0x36:
    BRANCH(EFLAG3() != 0);
    cycles += 2;
    break;
// **************** B4 @1 ***************
case 0x37:
    BRANCH(EFLAG4() != 0);
    cycles += 2;
    break;
// **************** SKP ***************
case 0x38:
    ADDRP(1);
    cycles += 2;
    break;
// **************** BNQ @1 ***************
case 0x39:
    BRANCH(Q == 0);
    cycles += 2;
    break;
// **************** BNZ @1 ***************
case 0x3a:
    BRANCH(D != 0);
    cycles += 2;
    break;
// **************** BNF @1 ***************
case 0x3b:
    BRANCH(DF == 0);
    cycles += 2;
    break;
// **************** BN1 @1 ***************
case 0x3c:
    BRANCH(EFLAG1() == 0);
    cycles += 2;
    break;
// **************** BN2 @1 ***************
case 0x3d:
    BRANCH(EFLAG2() == 0);
    cycles += 2;
    break;
// **************** BN3 @1 ***************
case 0x3e:
    BRANCH(EFLAG3() == 0);
    cycles += 2;
    break;
// **************** BN4 @1 ***************
case 0x3f:
    BRANCH(EFLAG4() == 0);
    cycles += 2;
    break;
// **************** LDA R0 ***************
case 0x40:
case 0x41:
case 0x42:
case 0x43:
case 0x44:
case 0x45:
case 0x46:
case 0x47:
case 0x48:
case 0x49:
case 0x4a:
case 0x4b:
case 0x4c:
case 0x4d:
case 0x4e:
case 0x4f:
    MA = *pReg;INCREG();READ();D = MB;
    cycles += 2;
    break;
// **************** STR R0 ***************
case 0x50:
case 0x51:
case 0x52:
case 0x53:
case 0x54:
case 0x55:
case 0x56:
case 0x57:
case 0x58:
case 0x59:
case 0x5a:
case 0x5b:
case 0x5c:
case 0x5d:
case 0x5e:
case 0x5f:
    MA = *pReg;MB = D;WRITE();
    cycles += 2;
    break;
// **************** IRX ***************
case 0x60:
    INCRX();
    cycles += 2;
    break;
// **************** OUT 1 ***************
case 0x61:
    READMX();OUTPORT1(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 2 ***************
case 0x62:
    READMX();OUTPORT2(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 3 ***************
case 0x63:
    READMX();OUTPORT3(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 4 ***************
case 0x64:
    READMX();OUTPORT4(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 5 ***************
case 0x65:
    READMX();OUTPORT5(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 6 ***************
case 0x66:
    READMX();OUTPORT6(MB);INCRX();
    cycles += 2;
    break;
// **************** OUT 7 ***************
case 0x67:
    READMX();OUTPORT7(MB);INCRX();
    cycles += 2;
    break;
// **************** NOP68 ***************
case 0x68:
    ;;
    cycles += 2;
    break;
// **************** INP 1 ***************
case 0x69:
    D = MB = INPORT1();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 2 ***************
case 0x6a:
    D = MB = INPORT2();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 3 ***************
case 0x6b:
    D = MB = INPORT3();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 4 ***************
case 0x6c:
    D = MB = INPORT4();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 5 ***************
case 0x6d:
    D = MB = INPORT5();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 6 ***************
case 0x6e:
    D = MB = INPORT6();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** INP 7 ***************
case 0x6f:
    D = MB = INPORT7();MA = R[X];WRITE();
    cycles += 2;
    break;
// **************** RET ***************
case 0x70:
    INTRET(1);
    cycles += 2;
    break;
// **************** DIS ***************
case 0x71:
    INTRET(0);
    cycles += 2;
    break;
// **************** LDXA ***************
case 0x72:
    READMX();INCRX();D = MB;
    cycles += 2;
    break;
// **************** STXD ***************
case 0x73:
    MA = R[X];DECRX();MB = D;WRITE();
    cycles += 2;
    break;
// **************** ADC ***************
case 0x74:
    READMX();ADD();
    cycles += 2;
    break;
// **************** SDB ***************
case 0x75:
    READMX();SUB(MB,D);
    cycles += 2;
    break;
// **************** RSHR ***************
case 0x76:
    MB = D;D = (D >> 1) | (DF << 7);DF = MB & 1;
    cycles += 2;
    break;
// **************** SMB ***************
case 0x77:
    READMX();SUB(D,MB);
    cycles += 2;
    break;
// **************** SAV ***************
case 0x78:
    MA = R[X];MB = T;WRITE();
    cycles += 2;
    break;
// **************** MARK ***************
case 0x79:
    T = (X << 4) | P;MA = R[2];MB = T;WRITE();X = P;DECR2();
    cycles += 2;
    break;
// **************** REQ ***************
case 0x7a:
    if (Q != 0) { Q = 0;SETQOFF(); };
    cycles += 2;
    break;
// **************** SEQ ***************
case 0x7b:
    if (Q == 0) { Q = 1;SETQON(); };
    cycles += 2;
    break;
// **************** ADCI @1 ***************
case 0x7c:
    FETCH8();ADD();
    cycles += 2;
    break;
// **************** SDBI @1 ***************
case 0x7d:
    FETCH8();SUB(MB,D);
    cycles += 2;
    break;
// **************** RSHL ***************
case 0x7e:
    MB = (D >> 7) & 1;D = TOD((D << 1) | DF);DF = MB;
    cycles += 2;
    break;
// **************** SMBI @1 ***************
case 0x7f:
    FETCH8();SUB(D,MB);
    cycles += 2;
    break;
// **************** GLO R0 ***************
case 0x80:
case 0x81:
case 0x82:
case 0x83:
case 0x84:
case 0x85:
case 0x86:
case 0x87:
case 0x88:
case 0x89:
case 0x8a:
case 0x8b:
case 0x8c:
case 0x8d:
case 0x8e:
case 0x8f:
    D = TOD(*pReg);
    cycles += 2;
    break;
// **************** GHI R0 ***************
case 0x90:
case 0x91:
case 0x92:
case 0x93:
case 0x94:
case 0x95:
case 0x96:
case 0x97:
case 0x98:
case 0x99:
case 0x9a:
case 0x9b:
case 0x9c:
case 0x9d:
case 0x9e:
case 0x9f:
    D = TOD(*pReg >> 8);
    cycles += 2;
    break;
// **************** PLO R0 ***************
case 0xa0:
case 0xa1:
case 0xa2:
case 0xa3:
case 0xa4:
case 0xa5:
case 0xa6:
case 0xa7:
case 0xa8:
case 0xa9:
case 0xaa:
case 0xab:
case 0xac:
case 0xad:
case 0xae:
case 0xaf:
    *pReg = (*pReg & 0xFF00) | D;
    cycles += 2;
    break;
// **************** PHI R0 ***************
case 0xb0:
case 0xb1:
case 0xb2:
case 0xb3:
case 0xb4:
case 0xb5:
case 0xb6:
case 0xb7:
case 0xb8:
case 0xb9:
case 0xba:
case 0xbb:
case 0xbc:
case 0xbd:
case 0xbe:
case 0xbf:
    *pReg = (*pReg & 0x00FF) | (((WORD16)D) << 8);
    cycles += 2;
    break;
// **************** LBR @2 ***************
case 0xc0:
    LONGBRANCH(1);
    cycles += 3;
    break;
// **************** LBQ @2 ***************
case 0xc1:
    LONGBRANCH(Q != 0);
    cycles += 3;
    break;
// **************** LBZ @2 ***************
case 0xc2:
    LONGBRANCH(D == 0);
    cycles += 3;
    break;
// **************** LBDF @2 ***************
case 0xc3:
    LONGBRANCH(DF != 0);
    cycles += 3;
    break;
// **************** NOP ***************
case 0xc4:
    ;;
    cycles += 3;
    break;
// **************** LSNQ ***************
case 0xc5:
    LONGSKIP(Q == 0);
    cycles += 3;
    break;
// **************** LSNZ ***************
case 0xc6:
    LONGSKIP(D != 0);
    cycles += 3;
    break;
// **************** LSNF ***************
case 0xc7:
    LONGSKIP(DF == 0);
    cycles += 3;
    break;
// **************** LSKP ***************
case 0xc8:
    ADDRP(2);
    cycles += 3;
    break;
// **************** LBNQ @2 ***************
case 0xc9:
    LONGBRANCH(Q == 0);
    cycles += 3;
    break;
// **************** LBNZ @2 ***************
case 0xca:
    LONGBRANCH(D != 0);
    cycles += 3;
    break;
// **************** LBNF @2 ***************
case 0xcb:
    LONGBRANCH(DF == 0);
    cycles += 3;
    break;
// **************** LSIE ***************
case 0xcc:
    LONGSKIP(IE != 0);
    cycles += 3;
    break;
// **************** LSQ ***************
case 0xcd:
    LONGSKIP(Q != 0);
    cycles += 3;
    break;
// **************** LSZ ***************
case 0xce:
    LONGSKIP(D == 0);
    cycles += 3;
    break;
// **************** LSDF ***************
case 0xcf:
    LONGSKIP(DF != 0);
    cycles += 3;
    break;
// **************** SEP R0 ***************
case 0xd0:
case 0xd1:
case 0xd2:
case 0xd3:
case 0xd4:
case 0xd5:
case 0xd6:
case 0xd7:
case 0xd8:
case 0xd9:
case 0xda:
case 0xdb:
case 0xdc:
case 0xdd:
case 0xde:
case 0xdf:
    P = opcode & 0xF;
    cycles += 2;
    break;
// **************** SEX R0 ***************
case 0xe0:
case 0xe1:
case 0xe2:
case 0xe3:
case 0xe4:
case 0xe5:
case 0xe6:
case 0xe7:
case 0xe8:
case 0xe9:
case 0xea:
case 0xeb:
case 0xec:
case 0xed:
case 0xee:
case 0xef:
    X = opcode & 0xF;
    cycles += 2;
    break;
// **************** LDX ***************
case 0xf0:
    READMX();D = MB;
    cycles += 2;
    break;
// **************** OR ***************
case 0xf1:
    READMX();D |= MB;
    cycles += 2;
    break;
// **************** AND ***************
case 0xf2:
    READMX();D &= MB;
    cycles += 2;
    break;
// **************** XOR ***************
case 0xf3:
    READMX();D ^= MB;
    cycles += 2;
    break;
// **************** ADD ***************
case 0xf4:
    DF = 0;READMX();ADD();
    cycles += 2;
    break;
// **************** SD ***************
case 0xf5:
    DF = 1;READMX();SUB(MB,D);
    cycles += 2;
    break;
// **************** SHR ***************
case 0xf6:
    DF = D & 1;D = (D >> 1) & 0x7F;
    cycles += 2;
    break;
// **************** SM ***************
case 0xf7:
    DF = 1;READMX();SUB(D,MB);
    cycles += 2;
    break;
// **************** LDI @1 ***************
case 0xf8:
    FETCH8();D = MB;
    cycles += 2;
    break;
// **************** ORI @1 ***************
case 0xf9:
    FETCH8();D |= MB;
    cycles += 2;
    break;
// **************** ANI @1 ***************
case 0xfa:
    FETCH8();D &= MB;
    cycles += 2;
    break;
// **************** XRI @1 ***************
case 0xfb:
    FETCH8();D ^= MB;
    cycles += 2;
    break;
// **************** ADI @1 ***************
case 0xfc:
    DF = 0;FETCH8();ADD();
    cycles += 2;
    break;
// **************** SDI @1 ***************
case 0xfd:
    DF = 1;FETCH8();SUB(MB,D);
    cycles += 2;
    break;
// **************** SHL ***************
case 0xfe:
    DF = (D >> 7) & 1;D = TOD(D << 1);
    cycles += 2;
    break;
// **************** SMI @1 ***************
case 0xff:
    DF = 1;FETCH8();SUB(D,MB);
    cycles += 2;
    break;
