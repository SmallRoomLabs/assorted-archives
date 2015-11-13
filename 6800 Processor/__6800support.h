#define BRANCHIF(cond)  FETCH();if (cond) { BRANCH(MB); }
#define BRANCH(n)    PC = ((n) & 0x80) ? (pc + (n) - 256) : (pc + (n))
#define FETCHWORD()   { FETCH();temp16 = (MB << 8);FETCH(); temp16 |= MB; }
#define READWORD()   { READ();temp16 = (MB << 8);MA++;READ();temp16 |= MB; }
#define PULLBYTE()   { MA = ++SP;READ(); }
#define PUSHBYTE(n)   { MA = SP--;WRITE(); }
#define PUSHWORD()   { PUSHBYTE(temp16);PUSHBYTE((temp16) >> 8); }
#define PULLWORD()     { PULLBYTE();temp16 = MB << 8;PULLBYTE(); temp16 |= MB; }
static BYTE8 A,B;
static WORD16 IX,SP,PC;
static BYTE8 carryFlag,overflowFlag,halfCarryFlag,nValue,zValue,interruptMaskFlag;
static BYTE8 MB,bsrTemp;
static WORD16 temp16,MA;
static BYTE8 getCCR(void) {
 BYTE8 ccr = carryFlag | (overflowFlag << 1) | (interruptMaskFlag << 4) | (halfCarryFlag << 5) | 0xC0;
 if (zValue == 0) ccr |= 0x04;
 if (nValue & 0x80) ccr |= 0x08;
 return ccr;
}
static void putCCR(BYTE8 ccr) {
 carryFlag = ccr & 1;
 overflowFlag = (ccr >> 1) & 1;
 interruptMaskFlag = (ccr >> 4) & 1;
 halfCarryFlag = (ccr >> 5) & 1;
 nValue = (ccr & 0x08) << 4;
 zValue = (ccr & 0x04) ? 0 : 1;
}
static void CPUReset(void) {
 MA = 0xFFFE;READWORD();PC = temp16;
 carryFlag = carryFlag & 1;halfCarryFlag = halfCarryFlag & 1;interruptMaskFlag = interruptMaskFlag & 1;
 overflowFlag = overflowFlag & 1;nValue = zValue;
}
static void _CPUSoftwareInterrupt( void) {
 PUSHWORD(PC);
 PUSHWORD(IX);
 PUSHBYTE(A);
 PUSHBYTE(B);
 PUSHBYTE(getCCR());
 interruptMask = 1;
 MA = 0xFFFA;READWORD();PC = temp16;
}
static BYTE8 _CPUAdd8Bit(BYTE8 n1,BYTE8 n2) {
 WORD16 result = n1 + n2 + carry;
 carryFlag = result >> 8;
 halfcarryFlag = ((n1 & 0xF) + (n2 & 0xF) + carry) >> 4;
 overflowFlag = ((n1 & 0x80) == (n2 & 0x80) &&
       (n1 & 0x80) != (((BYTE8)result) & 0x80)) ? 1 : 0;
 nValue = zValue = result;
 return result;
}
static BYTE8 _CPUSub8Bit(BYTE8 n1,BYTE8 n2) {
 WORD16 result = n1 - n2 - carry;
 carryFlag = (result >> 8) & 1;
 overflowFlag = ((n1 & 0x80) != (n2 & 0x80) &&
       (n2 & 0x80) == (((BYTE8)result) & 0x80)) ? 1 : 0;
 nValue = zValue = result;
 return result;
}
static void _CPUCmp16Bit(WORD16 w1,WORD16 w2) {
 WORD16 result = w1 - w2;
 overflowFlag = ((w1 & 0x8000) != (w2 & 0x8000) &&
       (w2 & 0x8000) == (((WORD16)result) & 0x8000)) ? 1 : 0;
 nValue = result >> 8;
 zValue = (result >> 8) | result;
}
static void _CPUDecimalAdjust(void) {
 BYTE8 correction = 0;
 if (A > 0x99 || carryFlag != 0) {
  correction = 0x60;
  carryFlag = 1;
 } else {
  carryFlag = 0;
 }
 if ((A & 0xF) > 9 || halfcarryFlag != 0) {
  correction += 6;
 }
 A = A + correction;
 nValue = zValue = A;
}
static BYTE8 _CPUASLShift(BYTE8 n) {
 carryFlag = n >> 7;
 nValue = zValue = n << 1;
 overflowFlag = carryFlag ^ (nValue >> 7);
 return nValue;
}
static BYTE8 _CPUASRShift(BYTE8 n) {
 carryFlag = n & 1;
 nValue = zValue = (n >> 1) | (n & 0x80);
 overflowFlag = carryFlag ^ (nValue >> 7);
 return nValue;
}
static BYTE8 _CPULSRShift(BYTE8 n) {
 carryFlag = n & 1;
 nValue = zValue = (n >> 1);
 overflowFlag = carryFlag ^ (nValue >> 7);
 return nValue;
}
static BYTE8 _CPUROLRotate(BYTE8 n) {
 BYTE8 newcarryFlag = n >> 7;
 nValue = zValue = (n << 1) | carry;
 carryFlag = newCarry;
 overflowFlag = carryFlag ^ (nValue >> 7);
 return nValue;
}
static BYTE8 _CPURORRotate(BYTE8 n) {
 BYTE8 newcarryFlag = n & 1;
 nValue = zValue = (n >> 1) | (carryFlag << 7);
 carryFlag = newCarry;
 overflowFlag = carryFlag ^ (nValue >> 7);
 return nValue;
}