// Simple Hello world demo using H/W SPI
#include <SPI.h>
#include <Wire.h>
#include "SSD1306_OLED.h"

// Hardware SPI pins include D11=Data and D13=Clk
#define OLED_DC 5
#define OLED_CS 6
#define OLED_RST 9

SSD1306ElfDisplay oled(1);

uint8_t screen[256];

void plot(int x,int y,int c) {
	int offset = (x / 8) + y * 8;
	if (c) {
		screen[offset] |= (0x80 >> (x & 7));
	} else {
		screen[offset] &= (0x80 >> (x & 7)) ^ 0xFF;
	}
}

//------------------------------------------------------------------------------
void setup() {
  
    oled.init();
    oled.clear();
    for (int x = 0;x < 64;x++) {
    	plot(x,x/2,1);
    	plot(63-x,x/2,1);
    }
    oled.refresh(screen);
    oled.clear();
}
//------------------------------------------------------------------------------

int n = 0;

void loop() {  
	n++;
    oled.displayBinary(n);
//	plot(n/32&63,n&31,rand()&1);
	delay(300);
//	if ((n & 31) == 0) oled.refresh(screen);
}
