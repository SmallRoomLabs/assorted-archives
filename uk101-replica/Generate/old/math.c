//
//	Math test
//

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE8;
typedef unsigned short WORD16;

static BYTE8 carryFlag,overflowFlag;

#include "arithmetic.h"

#define ASSERT(x) if (!(x)) exit(fprintf(stderr,"Line %d\n",__LINE__))
#define ISDECIMAL(n) ((n & 0xF) <= 9 && (n >> 4) <= 9)
#define FROMDEC(n) ((n & 0xF) + 10 * (n >> 4))

int main() {
	srand(42);
	WORD16 w;
	ASSERT(sizeof(w) == 2);
	for (int i = 0;i < 256*256*256;i++) {

		BYTE8 n1 = rand();
		ASSERT(sizeof(n1) == 1);
		BYTE8 n2 = rand();
		int i1 = (n1 & 0x80) ? n1 - 256 : n1;
		int i2 = (n2 & 0x80) ? n2 - 256 : n2;
		//printf("%d %d %d %d\n",n1,n2,i1,i2); 

		carryFlag = rand() & 1; 																		// Test binary addition of 8 bit numbers.
		int result = i1 + i2 + carryFlag;
		int uresult = n1 + n2 + carryFlag;
		BYTE8 r = add8Bit(n1,n2,0);
		ASSERT((r & 0xFF) == (result & 0xFF));
		//printf("%d %d\n",result,carryFlag);
		ASSERT(carryFlag == ((uresult >= 0x100) ? 1 : 0));
		ASSERT(overflowFlag == ((result > 127  || result < -128) ? 1 : 0));

		carryFlag = rand() & 1; 																		// Test binary subtraction 
		result = i1 - i2 - (carryFlag ^ 1);
		uresult = n1 - n2 - (carryFlag ^ 1);
		r = sub8Bit(n1,n2,0);
		ASSERT((r & 0xFF) == (result & 0xFF));
		//printf("%d %d\n",carryFlag,uresult);
		ASSERT(carryFlag == ((uresult < 0) ? 0 : 1));
		ASSERT(overflowFlag == ((result > 127  || result < -128) ? 1 : 0));

		if (ISDECIMAL(n1) && ISDECIMAL(n2)) {															// Only test valid BCD 6502 values.
			BYTE8 d1 = FROMDEC(n1);
			BYTE8 d2 = FROMDEC(n2);

			//printf("Decimal %x %x %d %d\n",n1,n2,d1,d2);

			carryFlag = rand() & 1;																		// Test decimal addition.
			result = d1 + d2 + carryFlag;

			r = add8Bit(n1,n2,1);
			//printf("%d %d\n",result,FROMDEC(r));
			ASSERT(result % 100 == FROMDEC(r)); 														// We don't check overflow.
			ASSERT(carryFlag == ((result >= 100) ? 1 : 0));

			carryFlag = rand() & 1;																		// Test decimal subtraction.
			result = d1 - d2 - (carryFlag ^ 1);
			r = sub8Bit(n1,n2,1);
			ASSERT((result+100) % 100 == FROMDEC(r));
			//printf("%d %d\n",result,FROMDEC(r));
			ASSERT(carryFlag == (result < 0) ? 0 : 1);

		}
	}
	printf(" ** Completed ** \n");
	return 0;
}