static BYTE8   D,DF,X,P,Q,T,IE,opcode,MB;
static WORD16   R[16],MA,LBA,cycles;
#define FETCH8()  MA = R[P];R[P] = (R[P]+1) & 0xFFFF;READ()
#define FETCH16()  FETCH8();LBA = MB;FETCH8();LBA |= (((WORD16)MB) << 8)
#define READMX()  MA = R[X];READ();
static void CPU1802Reset(void) {
 Q = 0;IE = 1;
 X = P = R(0) = 0;
 cycles = 0;
 UPDATEQ(0);
}
#define ADD() MA = D+MB+DF;DF = MB >> 8;D = MA & 0xFF
#define SUB(n1,n2) MA = (n1)+((n2) ^ 0xFF)+DF;DF = MA >> 8;D = MA & 0xFF
#define BRANCH(t)  FETCH8();if (t) R[P] = (R[P] & 0xFF00) | MB
#define FETCH16()   MA = R[P];R[P] = (R[P]+2) & 0xFFFF;READ16()
#define LONGBRANCH(t) FETCH16();if (t) R[P] = LBA
#define LONGSKIP(t)  if (t) R[P] = (R[P]+2) & 0xFFFF
#define INTRET(f)   MA = R[X];READ();R[X] = (R[X]+1) & 0xFFFF;X = (MB >> 4) & 0xF;P = MB & 0xF;IE = f
static void CPU1802Interrupt(void) {
 if (IE != 0) {
  T = (X << 4) | P
  P = 1;X = 2;
  IE = 0;
 }
}