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
    oled.clear();
    for (int x = 0;x < 20;x++)
    	for (int y = 0;y < 8;y++)
    			oled.drawCharacter(x*6,y,x+y*16+32);
}
//------------------------------------------------------------------------------

int n = 0;

void loop() {  
}
