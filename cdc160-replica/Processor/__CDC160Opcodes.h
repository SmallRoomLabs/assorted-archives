/*** Generated Code, do not edit ***/

case 000: // err/err %d
    halt = 1;P = (P + 1) & 07777;cycles += 1;break;
case 001: // sha/sha %d
    CDC160Shift();P = (P + 1) & 07777;cycles += 1;break;
case 002: // lpn/and #%d
    A = A & (Z & 077);P = (P + 1) & 07777;cycles += 1;break;
case 003: // lsn/xor #%d
    A = A ^ (Z & 077);P = (P + 1) & 07777;cycles += 1;break;
case 004: // ldn/lda #%d
    A = (Z & 077);P = (P + 1) & 07777;cycles += 1;break;
case 005: // lcn/lca #%d
    A = (Z & 077) ^ 07777;P = (P + 1) & 07777;cycles += 1;break;
case 006: // adn/add #%d
    Z = Z & 077;ADD();P = (P + 1) & 07777;cycles += 1;break;
case 007: // sbn/sub #%d
    Z = (Z & 077) ^ 07777;ADD();P = (P + 1) & 07777;cycles += 1;break;
case 010: // lpd/and %d
    EACD();READ();A = A & Z;P = (P + 1) & 07777;cycles += 2;break;
case 011: // lpi/and @%d
    EACI();READ();A = A & Z;P = (P + 1) & 07777;cycles += 3;break;
case 012: // lpf/and %f
    EACF();READ();A = A & Z;P = (P + 1) & 07777;cycles += 2;break;
case 013: // lpb/and %b
    EACB();READ();A = A & Z;P = (P + 1) & 07777;cycles += 2;break;
case 014: // lsd/xor %d
    EACD();READ();A = A ^ Z;P = (P + 1) & 07777;cycles += 2;break;
case 015: // lsi/xor @%d
    EACI();READ();A = A ^ Z;P = (P + 1) & 07777;cycles += 3;break;
case 016: // lsf/xor %f
    EACF();READ();A = A ^ Z;P = (P + 1) & 07777;cycles += 2;break;
case 017: // lsb/xor %b
    EACB();READ();A = A ^ Z;P = (P + 1) & 07777;cycles += 2;break;
case 020: // ldd/lda %d
    EACD();READ();A = Z;P = (P + 1) & 07777;cycles += 2;break;
case 021: // ldi/lda @%d
    EACI();READ();A = Z;P = (P + 1) & 07777;cycles += 3;break;
case 022: // ldf/lda %f
    EACF();READ();A = Z;P = (P + 1) & 07777;cycles += 2;break;
case 023: // ldb/lda %b
    EACB();READ();A = Z;P = (P + 1) & 07777;cycles += 2;break;
case 024: // lcd/lca %d
    EACD();READ();A = Z ^ 07777;P = (P + 1) & 07777;cycles += 2;break;
case 025: // lci/lca @%d
    EACI();READ();A = Z ^ 07777;P = (P + 1) & 07777;cycles += 3;break;
case 026: // lcf/lca %f
    EACF();READ();A = Z ^ 07777;P = (P + 1) & 07777;cycles += 2;break;
case 027: // lcb/lca %b
    EACB();READ();A = Z ^ 07777;P = (P + 1) & 07777;cycles += 2;break;
case 030: // add/add %d
    EACD();READ();ADD();P = (P + 1) & 07777;cycles += 2;break;
case 031: // adi/add @%d
    EACI();READ();ADD();P = (P + 1) & 07777;cycles += 3;break;
case 032: // adf/add %f
    EACF();READ();ADD();P = (P + 1) & 07777;cycles += 2;break;
case 033: // adb/add %b
    EACB();READ();ADD();P = (P + 1) & 07777;cycles += 2;break;
case 034: // sbd/sub %d
    EACD();READ();Z = Z ^ 07777;ADD();P = (P + 1) & 07777;cycles += 2;break;
case 035: // sbi/sub @%d
    EACI();READ();Z = Z ^ 07777;ADD();P = (P + 1) & 07777;cycles += 3;break;
case 036: // sbf/sub %f
    EACF();READ();Z = Z ^ 07777;ADD();P = (P + 1) & 07777;cycles += 2;break;
case 037: // sbb/sub %b
    EACB();READ();Z = Z ^ 07777;ADD();P = (P + 1) & 07777;cycles += 2;break;
case 040: // std/sta %d
    EACD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 041: // sti/sta @%d
    EACI();Z = A;WRITE();P = (P + 1) & 07777;cycles += 4;break;
case 042: // stf/sta %f
    EACF();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 043: // stb/sta %b
    EACB();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 044: // srd/rml %d
    EACD();READ();A = Z;ROTATEA();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 045: // sri/rml @%d
    EACI();READ();A = Z;ROTATEA();Z = A;WRITE();P = (P + 1) & 07777;cycles += 4;break;
case 046: // srf/rml %f
    EACF();READ();A = Z;ROTATEA();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 047: // srb/rml %b
    EACB();READ();A = Z;ROTATEA();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 050: // rad/adm %d
    EACD();READ();ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 051: // rai/adm @%d
    EACI();READ();ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 4;break;
case 052: // raf/adm %f
    EACF();READ();ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 053: // rab/adm %b
    EACB();READ();ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 054: // aod/ild %d
    EACD();READ();A = 1;ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 055: // aoi/ild @%d
    EACI();READ();A = 1;ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 4;break;
case 056: // aof/ild %f
    EACF();READ();A = 1;ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 057: // aob/ild %b
    EACB();READ();A = 1;ADD();Z = A;WRITE();P = (P + 1) & 07777;cycles += 3;break;
case 060: // zjf/jz %f
    if (A == 0) { EACF();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 061: // nzf/jnz %f
    if (A != 0) { EACF();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 062: // pjf/jpl %f
    if ((A & 04000) == 0) { EACF();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 063: // njf/jmi %f
    if ((A & 04000) != 0) { EACF();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 064: // zjb/jz %b
    if (A == 0) { EACB();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 065: // nzb/jnz %b
    if (A != 0) { EACB();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 066: // pjb/jpl %b
    if ((A & 04000) == 0) { EACB();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 067: // njb/jmi %b
    if ((A & 04000) != 0) { EACB();P = S; } else { P = (P + 1) & 07777; };cycles += 1;break;
case 070: // jpi/jmp @%d
    EACI();P = S;cycles += 2;break;
case 071: // jfi/jmp @%f
    EACF();READ();P = Z;cycles += 2;break;
case 072: // inp/inp %f
    if (CDC160DataReady()) { CDC160BlockTransfer('I');P = (P + 1) & 07777;};cycles += 2;break;
case 073: // out/out %f
    CDC160BlockTransfer('O');P = (P + 1) & 07777;cycles += 2;break;
case 074: // otn/otc #%d
    Z = Z & 077;S = IOTRANSFER('O');P = (P + 1) & 07777;cycles += 1;break;
case 075: // exf/exf %f
    EACF();READ();IOEXTERNAL();P = (P + 1) & 07777;cycles += 2;break;
case 076: // ina/ina
    if (CDC160DataReady()) { S = IOTRANSFER('I');A = Z;P = (P + 1) & 07777;};cycles += 1;break;
case 077: // hlt/hlt %d
    halt = 1;P = (P + 1) & 07777;cycles += 1;break;
