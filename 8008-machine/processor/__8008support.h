static BYTE8   A,B,C,D,E,H,L,HaltFlag,Carry,PCIX,PSZValue,MB;
static WORD16   PCR[8],Cycles,MA,temp16;
#define HL()  MA = ((((WORD16)H) << 8) | L) & 0x3FFF
#define PCTR  PCR[PCIX]
#define POP()  PCIX = (PCIX-1) & 7
#define PUSH()  PCIX = (PCIX+1) & 7
#define FETCH()  MA = PCTR & 0x3FFF;READ();PCTR = (MA + 1) & 0x3FFF
#define FETCH16() FETCH();temp16 = MB;FETCH();temp16 |= (((WORD16)MB)<<8)
#define CYCLES(n) Cycles += n
static void CPU8008Reset(void) {
 A = B= C = D = E = H = L = HaltFlag = Carry = Cycles = PCIX = PSZValue = 0;
 for (BYTE8 n = 0;n < 8;n++) PCR[n] = 0;
}
static BYTE8 _ReadHL() {
 HL();READ();return MB;
}
static BYTE8 _Add(BYTE8 n) {
 temp16 = A + n + Carry; Carry = temp16 >> 8;
 return temp16 & 0xFF;
}
static BYTE8 _Sub(BYTE8 n) {
 temp16 = A - n - Carry; Carry = temp16 >> 8;
 return temp16 & 0xFF;
}
static BYTE8 _Parity(BYTE8 n) {
 BYTE8 p = 0;
 while (n != 0) {
  if ((n & 1) != 0) p ^= 1;
  n = n >> 1;
 }
 return p;
}