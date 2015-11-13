// **********************************************************************************************************************
//												8 bit Add with Carry In/Out
// **********************************************************************************************************************

static BYTE8 add8Bit(BYTE8 n1,BYTE8 n2,BYTE8 isDecimalMode) {
	WORD16 result;
 	BYTE8 r,t;
 	if (isDecimalMode) {
       	r = (n1 & 0xf)+(n2 & 0xf)+carryFlag;
        if (r > 9) r += 6;
        t = (n1 >> 4)+(n2 >> 4)+ (r >= 0x10 ? 1 : 0);
        if (t > 9) t += 6;
        result = (r & 0xF) | (t << 4);
        carryFlag = (t >= 0x10 ? 1 : 0);
    } else {
        result = n1 + n2 + carryFlag;
        overflowFlag = ((n1 & 0x80) == (n2 & 0x80) && (n1 & 0x80) != (result & 0x80)) ? 1 : 0;
        carryFlag = (result >> 8) & 1;
    }
 	return result & 0xFF;
}

// **********************************************************************************************************************
//											8 bit Subtract with Carry In/Out
// **********************************************************************************************************************

static BYTE8 sub8Bit(BYTE8 n1,BYTE8 n2,BYTE8 isDecimalMode) {
 	WORD16 result;
 	BYTE8 r,t;
 	if (isDecimalMode) {
 		r = (n1 & 0xf) - (n2 & 0xf) - (carryFlag ^ 1);
        if (r & 0x10) r -= 6;
 		t = (n1 >> 4) - (n2 >> 4) - ((r & 0x10)>>4);
        if (t & 0x10) t -= 6;
 		result = (r & 0xF) | (t << 4);
 		carryFlag = (t > 15) ? 0 : 1;
    } else {
        result = n1 + (n2 ^ 0xFF) + carryFlag;
 		carryFlag = (result >> 8) & 1;
 		overflowFlag = ((n1 & 0x80) != (n2 & 0x80) && (n1 & 0x80) != (result & 0x80)) ? 1 : 0;
 	}
 	return result & 0xFF;
}
