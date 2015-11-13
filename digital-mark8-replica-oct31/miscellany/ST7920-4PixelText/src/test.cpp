#include "ST7920LCDDriver.h"


const int MOSIpin = A1; // 11
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; // 10

// End of configuration section

// LCD
static ST7920LCDDriver lcd(SCLKpin, MOSIpin, 0);

// Initialization
void setup()
{
  pinMode(SSpin, OUTPUT);   // must do this before we use the lcd in SPI mode
  digitalWrite(SSpin, HIGH);
  lcd.begin(true);          
  lcd.clear();
  for (int x = 0;x < 8;x++) {
  	for (int y = 0;y < 10;y++) {
  		uint8_t b[4];
  		b[0] = (rand() & 63) + 32;
  		b[1] = (rand() & 63) + 32;
  		b[2] = (rand() & 63) + 32;
  		b[3] = (rand() & 63) + 32;
  		lcd.drawText(x,y*6+2,b); 
  	}
  }
}

void loop()
{
}


