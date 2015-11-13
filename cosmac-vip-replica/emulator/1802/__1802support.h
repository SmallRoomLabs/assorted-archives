static WORD16   *pReg;
#define FETCH8()  MA = R[P];ADDRP(1);READ()
#define FETCH16()  MA = R[P];ADDRP(2);READ16()
#define READMX()  MA = R[X];READ()
#define INCRX()  R[X] = (R[X]+1) & 0xFFFF
#define DECRX()  R[X] = (R[X]-1) & 0xFFFF
#define DECR2()  R[2] = (R[2]-1) & 0xFFFF
#define INCREG()  *pReg = (*pReg + 1) & 0xFFFF
#define DECREG()  *pReg = (*pReg - 1) & 0xFFFF
#define ADDRP(n)  R[P] = (R[P] + n) & 0xFFFF
#define TOD(n)  ((n) & 0xFF)
static void CPU1802Reset(void) {
 Q = 0;IE = 1;
 X = P = R[0] = 0;
 cycles = 0;
 SETQOFF();
}
#define ADD() MA = D+MB+DF;DF = MB >> 8;D = TOD(MA)
#define SUB(n1,n2) MA = (n1)+((n2) ^ 0xFF)+DF;DF = MA >> 8;D = TOD(MA)
#define BRANCH(t)  FETCH8();if (t) R[P] = (R[P] & 0xFF00) | MB
#define LONGBRANCH(t) FETCH16();if (t) R[P] = LBA
#define LONGSKIP(t)  if (t) ADDRP(2)
#define INTRET(f)   MA = R[X];READ();INCRX();X = (MB >> 4) & 0xF;P = MB & 0xF;IE = f
static void CPU1802Interrupt(void) {
 if (IE != 0) {
  T = (X << 4) | P;
  P = 1;X = 2;
  IE = 0;
 }
}