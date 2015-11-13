static BYTE8 	AC,SR,EX;
static WORD16 	P0,P1,P2,P3;
static WORD16  CYCLES;
static WORD16 	FRAMESTOSKIP;
static BYTE8 	OPCODE,OPERAND;
static WORD16 	EA,TEMP16;
void SCMPReset(void) {
AC = SR = EX = P0 = P1 = P2 = P3 = 0;
CYCLES = 0;FRAMESTOSKIP = 0;
}
#define INC1612(pr)		pr = (pr + 1) & 0xFFFF
void SCMPExecute(void) {
INC1612(P0);OPCODE = READ(P0);
if ((OPCODE & 0x80) != 0) {
INC1612(P0);OPERAND = READ(P0);
}
switch(OPCODE) {
#include "_opcodes.h"
}
}
static BYTE8 _SCMPBinaryAdd(BYTE8 n1,BYTE8 n2) {
WORD16 result = n1 + n2 + (SR >> 7);
SR = SR & 0x3F;
if ((result & 0x0100) != 0) SR = SR | 0x80;
if ((n1 & 0x80) == (n2 & 0x80)) {
if ((n1 & 0x80) != (result & 0x80)) SR = SR | 0x40;
}
return result & 0xFF;
}
static BYTE8 _SCMPDecimalAdd(BYTE8 n1,BYTE8 n2) {
WORD16 result = (n1 & 0x0F) + (n2 & 0x0F) + (SR >> 7);
SR = SR & 0x7F;
if (result > 0x09) {
result = (result - 0xA)+0x10;
}
result = result + (n1 & 0xF0) + (n2 & 0xF0);
if (result > 0x99) {
result = result - 0xA0;
SR = SR | 0x80;
}
}