// Simple Hello world demo using H/W SPI
#include <SPI.h>
#include <Wire.h>
#include "SSD1306_OLED.h"

// Hardware SPI pins include D11=Data and D13=Clk
#define OLED_DC 5
#define OLED_CS 6
#define OLED_RST 9

SSD1306Simple oled(1);

//------------------------------------------------------------------------------
void setup() {
  
    oled.init();

    uint8_t dat;
    for (uint8_t x = 0;x < 64;x++) {
    	oled.setPixel(x,x,1);
    	oled.setPixel(63-x,x,1);
    }
    oled.repaint();
}
//------------------------------------------------------------------------------

void loop() {  
}
