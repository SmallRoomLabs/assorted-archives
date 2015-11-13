/*** Generated Code, do not edit ***/

static WORD16 A,P,Z,S,cycles;
static BYTE8  halt;
static void CDC160Reset(void) {
 A = P = Z = S = cycles = 0;
 halt = 0;
}
#define ADD()   CDC160Add()
static void CDC160Add(void) {
 if (A != 07777 && Z != 07777) {
  A += Z;
  A += (A >> 12);
  if (A == 07777) A = 0;
 }
}
#define EACD()  S = Z & 077
#define EACI()  S = Z & 077;READ();S = Z
#define EACF()  S = ((P + (Z & 077)) & 0777)
#define EACB()  S = ((P - (Z & 077)) & 0777)
#define ROTATEA()  A = ((A << 1) | (A >> 11)) & 07777
static void CDC160Shift() {
 switch(Z) {
  case 0102:
   ROTATEA();break;
  case 0110:
   ROTATEA();ROTATEA();ROTATEA();break;
  case 0112:
   ROTATEA();Z = A;ROTATEA();ROTATEA();ADD();break;
 }
}
static void CDC160BlockTransfer(char direction) {
 BYTE8 error;
 EACF();
 READ();A = Z;
 P = (P + 1) & 07777;
 do {
  cycles += 2;
  if (direction == 'I') {
   error = IOTRANSFER('R');S = A;WRITE();
  } else {
   S = A;READ();error = IOTRANSFER('W');
  }
  A = (A + 1) & 07777;
  S = P;READ();
 } while (A != Z && error == 0);

}
static BYTE8 CDC160DataReady(void) {
 if (HWMIsStatusPending() != 0) return 1;
 BYTE8 error = IOTRANSFER('Q');
 return (error == 0);
}