#define BRANCHIF(cond)  temp8 = fetchByte();if (cond) { BRANCH(temp8); }
#define BRANCH(n)    { setProgramCounter(((n) & 0x80) ? (pc + (n) - 256) : (pc + (n))); }
#define FETCHWORDTEMP16() { temp16 = fetchByte();temp16 = (temp16 << 8) | fetchByte(); }
#define READWORDTEMP16(n) { temp16 = readByte(n);temp16 = (temp16 << 8) | readByte((n)+1); }
#define PULLBYTE()   readByte(++sp)
#define PUSHBYTE(n)   writeByte(sp--,n)
#define PUSHWORD(n)   { PUSHBYTE(n);PUSHBYTE((n) >> 8); }
#define PULLWORDTEMP16()   { temp16 = PULLBYTE();temp16 = (temp16 << 8) | PULLBYTE(); }
static BYTE8 a,b;
static WORD16 ix,sp,pc;
static BYTE8 carry,overflow,halfCarry,nValue,zValue,interruptMask;
static BYTE8 temp8;
static WORD16 temp16,eac;
static BYTE8 getCCR(void) {
 BYTE8 ccr = carry | (overflow << 1) | (interruptMask << 4) | (halfCarry << 5) | 0xC0;
 if (zValue == 0) ccr |= 0x04;
 if (nValue & 0x80) ccr |= 0x08;
 return ccr;
}
static void putCCR(BYTE8 ccr) {
 carry = ccr & 1;
 overflow = (ccr >> 1) & 1;
 interruptMask = (ccr >> 4) & 1;
 halfCarry = (ccr >> 5) & 1;
 nValue = (ccr & 0x08) << 4;
 zValue = (ccr & 0x04) ? 0 : 1;
}
static void resetProcessor(void) {
 READWORDTEMP16(0xFFFE);setProgramCounter(temp16);
 carry = carry & 1;halfCarry = halfCarry & 1;interruptMask = interruptMask & 1;
 overflow = overflow & 1;nValue = zValue;
}
static void softwareInterrupt(void) {
 PUSHWORD(pc);
 PUSHWORD(ix);
 PUSHBYTE(a);
 PUSHBYTE(b);
 PUSHBYTE(getCCR());
 interruptMask = 1;
 READWORDTEMP16(0xFFFA);
 setProgramCounter(temp16);
}
static BYTE8 ALUAdd8BitWithCarry(BYTE8 n1,BYTE8 n2) {
 WORD16 result = n1 + n2 + carry;
 carry = result >> 8;
 halfCarry = ((n1 & 0xF) + (n2 & 0xF) + carry) >> 4;
 overflow = ((n1 & 0x80) == (n2 & 0x80) &&
       (n1 & 0x80) != (((BYTE8)result) & 0x80)) ? 1 : 0;
 nValue = zValue = result;
 return result;
}

static BYTE8 ALUSub8BitWithCarry(BYTE8 n1,BYTE8 n2) {
 WORD16 result = n1 - n2 - carry;
 carry = (result >> 8) & 1;
 overflow = ((n1 & 0x80) != (n2 & 0x80) &&
       (n2 & 0x80) == (((BYTE8)result) & 0x80)) ? 1 : 0;
 nValue = zValue = result;
 return result;
}

static void ALUCompare16Bit(WORD16 w1,WORD16 w2) {
 WORD16 result = w1 - w2;
 overflow = ((w1 & 0x8000) != (w2 & 0x8000) &&
       (w2 & 0x8000) == (((WORD16)result) & 0x8000)) ? 1 : 0;
 nValue = result >> 8;
 zValue = (result >> 8) | result;
}

static void ALUDecimalAdjust(void) {
 BYTE8 correction = 0;
 if (a > 0x99 || carry != 0) {
  correction = 0x60;
  carry = 1;
 } else {
  carry = 0;
 }
 if ((a & 0xF) > 9 || halfCarry != 0) {
  correction += 6;
 }
 a = a + correction;
 nValue = zValue = a;
}

static BYTE8 ALUASLShift(BYTE8 n) {
 carry = n >> 7;
 nValue = zValue = n << 1;
 overflow = carry ^ (nValue >> 7);
 return nValue;
}

static BYTE8 ALUASRShift(BYTE8 n) {
 carry = n & 1;
 nValue = zValue = (n >> 1) | (n & 0x80);
 overflow = carry ^ (nValue >> 7);
 return nValue;
}

static BYTE8 ALULSRShift(BYTE8 n) {
 carry = n & 1;
 nValue = zValue = (n >> 1);
 overflow = carry ^ (nValue >> 7);
 return nValue;
}

static BYTE8 ALUROLRotate(BYTE8 n) {
 BYTE8 newCarry = n >> 7;
 nValue = zValue = (n << 1) | carry;
 carry = newCarry;
 overflow = carry ^ (nValue >> 7);
 return nValue;
}

static BYTE8 ALURORRotate(BYTE8 n) {
 BYTE8 newCarry = n & 1;
 nValue = zValue = (n >> 1) | (carry << 7);
 carry = newCarry;
 overflow = carry ^ (nValue >> 7);
 return nValue;
}
