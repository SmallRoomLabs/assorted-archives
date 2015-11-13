// Simple Hello world demo using H/W SPI
#include <SPI.h>
#include <Wire.h>
#include "SSD1306Simple.h"

// Hardware SPI pins include D11=Data and D13=Clk
#define OLED_DC 5
#define OLED_CS 6
#define OLED_RST 9

SSD1306Simple oled(1);

//------------------------------------------------------------------------------
void setup() {
  
// Initialize, optionally clear the screen
    oled.init();

    uint8_t dat;
    for (int i = 0;i < 128;i++) {
    	dat = i|0x80;
    	oled.loadColumn(i,0,&dat,7);
    }
}
//------------------------------------------------------------------------------

void loop() {  
}
