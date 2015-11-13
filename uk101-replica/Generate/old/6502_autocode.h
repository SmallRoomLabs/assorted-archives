#define PUSH(n) 	WRITE01(0x100+(s--),n)
#define POP(n) 	READ01(0x100+(++s))
#define BRANCH(t) 	{ temp8 = FETCH();if (t) { if (temp8 & 0x80) { SETPC(pc+temp8-256); } else { SETPC(pc+temp8); } } }
static BYTE8 a,x,y,s,carryFlag,interruptDisableFlag,breakFlag,decimalFlag,overflowFlag,sValue,zValue;
static WORD16 pc;
static BYTE8 temp8;
static WORD16 eac,temp16;
static void explodeFlagRegister(BYTE8 f) {
	carryFlag = f & 1;
	zValue = (f & 2);
	interruptDisableFlag = (f >> 2) & 1;
	decimalFlag = (f >> 3) & 1;
	breakFlag = (f >> 4) & 1;
	overflowFlag = (f >> 6) & 1;
	sValue = f & 0x80;
}
static BYTE8 constructFlagRegister(void) {
	BYTE8 f = 0x20 | carryFlag | (interruptDisableFlag << 2) | (decimalFlag << 3) | (breakFlag << 4) | (overflowFlag << 6);
	if (zValue == 0) f |= 0x02;
	if (sValue & 0x80) f |= 0x80;
	return f;
}
static BYTE8 aslCode(BYTE8 n) {
	carryFlag = (n >> 7);
	n = sValue = zValue = (n << 1);
	return n;
}
static BYTE8 lsrCode(BYTE8 n) {
	carryFlag = n & 1;
	n = sValue = zValue = (n >> 1) & 0x7F;
	return n;
}
static BYTE8 rolCode(BYTE8 n) {
	BYTE8 newCarry = (n >> 7);
	n = sValue = zValue = (n << 1) | carryFlag;
	carryFlag = newCarry;
	return n;
}
static BYTE8 rorCode(BYTE8 n) {
	BYTE8 newCarry = n & 1;
	n = sValue = zValue = (n >> 1) | (carryFlag << 7);
	carryFlag = newCarry;
	return n;
}
static void bitCode(BYTE8 n) {
	zValue = ((n & a) == 0);
	sValue = n & 0x80;
	overflowFlag = (n & 0x40) ? 1 : 0;
}
static void executeInterrupt(WORD16 vector,BYTE8 setBreakFlag) {
	PUSH(pc >> 8);PUSH(pc & 0xFF);
	PUSH(constructFlagRegister());
	if (setBreakFlag) breakFlag = 1;
	SETPC(READWORD(vector));
	interruptDisableFlag = 1;
}
static void brkCode(void) {
	pc++;
	executeInterrupt(0xFFFE,0);
}
static void nmiCode(void) {
	executeInterrupt(0xFFFA,1);
}
static void irqCode(void) {
	if (interruptDisableFlag == 0) executeInterrupt(0xFFFE,1);
}
static void resetProcessor(void) {
	interruptDisableFlag = 1;
	carryFlag &= 1;overflowFlag &= 1;
	breakFlag &= 1;decimalFlag &= 1;
	SETPC(READWORD(0xFFFC));
}
