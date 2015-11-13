#include "ST7920LCDDriver.h"


const int MOSIpin = A1; // 11
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; // 10

// End of configuration section

// LCD
static ST7920LCDDriver lcd(SCLKpin, MOSIpin, 0);

uint8_t b[14] = { 0xFF,0xFF,0xAA,0xAA,0xFF,0xFF,0xF0,0xF0,0xCC,0xCC,0xFF,0xFF,0x40,0x02 };

// Initialization
void setup()
{
  pinMode(SSpin, OUTPUT);   // must do this before we use the lcd in SPI mode
  digitalWrite(SSpin, HIGH);
  lcd.begin(true);          
  lcd.clear();
  for (int i = 0;i < 64;i++)
    lcd.draw((i >> 3) & 1,i,b,6);
}

void loop()
{
}


