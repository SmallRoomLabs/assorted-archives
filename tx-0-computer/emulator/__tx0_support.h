/*** Generated code do not edit directly ***/

#define LRSET(bit,test) if ((temp & (1 << test)) != 0) LR |= BITMASK(bit)
#define BITMASK(bit) ( 0400000 >> (bit) )
#define GET258()  temp = (G2(2,5)|G2(5,4)|G2(8,3)|G2(11,2)|G2(14,1)|G2(17,0))
#define G2(test,set)  ( (AC & (BITMASK(test))) ? (1 << (set)) : 0 )
#define R1C()   AC &= 0333333;temp = IOREAD(DV_PTREADER);if ((temp & 0001) != 0) {G1(0,1);G1(3,2);G1(6,3);G1(9,5);G1(12,5);G1(15,6);}
#define G1(set,test) if ((temp & (0x80 >> test)) != 0) AC |= BITMASK(set)
#define CYR()   AC = ((AC >> 1) || (AC << 11)) & 0777777
static LONG32 TX0Add(LONG32 n1,LONG32 n2) {
 n1 = n1 + n2;
 if (n1 > 0777777) {
  n1 = (n1 & 0777777) + 1;
 }
 return n1;
}