/*** Generated code do not edit directly ***/

/*** (Mandatory) at time 0.0 code = 600000 ***/

    MBR = 0;
/*** (Mandatory) at time 0.8 code = 600000 ***/

    temp = IOREAD(DV_KEYBOARD);
    if (temp != 0) {LR = (LR & 0666666) | 0400000;
    LRSET(2,5);
    LRSET(5,4);
    LRSET(8,3);
    LRSET(11,2);
    LRSET(14,1);
    LRSET(17,0);
    };
/*** CLL at time 0.8 code = 700000 ***/

if ((Instruction & 0100000) == 0100000) {
    AC &= 0000777;
}

/*** CLR at time 0.8 code = 640000 ***/

if ((Instruction & 0040000) == 0040000) {
    AC &= 0777000;
}

/*** IOS at time 0.8 code = 620000 ***/

if ((Instruction & 0030000) == 0020000) {

}

/*** P7H at time 0.8 code = 607000 ***/

if ((Instruction & 0007000) == 0007000) {
    GET258();
    IOWRITE(DV_PTPUNCH,(temp << 1)|0001);
}

/*** P6H at time 0.8 code = 606000 ***/

if ((Instruction & 0007000) == 0006000) {
    GET258();
    IOWRITE(DV_PTPUNCH,temp << 1);
}

/*** PNT at time 0.8 code = 604000 ***/

if ((Instruction & 0007000) == 0004000) {
    GET258();
    IOWRITE(DV_PRINTER,temp);
}

/*** R1C at time 0.8 code = 601000 ***/

if ((Instruction & 0007000) == 0001000) {
    R1C();
}

/*** R3C at time 0.8 code = 603000 ***/

if ((Instruction & 0007000) == 0003000) {
    R1C();
    CYR();
    R1C();
    CYR();
    R1C();
    CYR();
}

/*** DIS at time 0.8 code = 602000 ***/

if ((Instruction & 0007000) == 0002000) {
    IOWRITE(DV_SCOPE,AC);
}

/*** PEN at time 1.1 code = 600100 ***/

if ((Instruction & 0000104) == 0000100) {
    AC &= 0177777;
    if (IOREAD(DV_LIGHTPEN1)) AC |= 0400000;
    if (IOREAD(DV_LIGHTPEN2)) AC |= 0200000;
}

/*** TAC at time 1.1 code = 600004 ***/

if ((Instruction & 0000104) == 0000004) {
    AC = AC | IOREAD(DV_TOGGLESWACC);
}

/*** COM at time 1.2 code = 600040 ***/

if ((Instruction & 0000040) == 0000040) {
    AC ^= 0777777;
}

/*** AMB at time 1.2 code = 600001 ***/

if ((Instruction & 0000003) == 0000001) {
    MBR = AC;
}

/*** TBR at time 1.2 code = 600003 ***/

if ((Instruction & 0000003) == 0000003) {
    MBR |= IOREAD(DV_TOGGLESWBUF);
}

/*** MLR at time 1.3 code = 600200 ***/

if ((Instruction & 0000600) == 0000200) {
    LR = MBR;
}

/*** LMB at time 1.3 code = 600002 ***/

if ((Instruction & 0000003) == 0000002) {
    MBR = LR;
}

/*** SHR at time 1.4 code = 600400 ***/

if ((Instruction & 0000600) == 0000400) {
    AC = (AC >> 1) | (AC & 0400000);
}

/*** CYR at time 1.4 code = 600600 ***/

if ((Instruction & 0000600) == 0000600) {
    CYR();
}

/*** PAD at time 1.4 code = 600020 ***/

if ((Instruction & 0000020) == 0000020) {
    AC = AC ^ MBR;
}

/*** CRY at time 1.7 code = 600010 ***/

if ((Instruction & 0000010) == 0000010) {
    AC = AC ^ MBR;
    AC = TX0Add(AC,MBR);
}

/*** HLT at time 1.8 code = 630000 ***/

if ((Instruction & 0030000) == 0030000) {
    HALT = 1;
}

