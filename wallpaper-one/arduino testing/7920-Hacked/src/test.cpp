// ST7920-based 128x64 pixel graphic LCD demo program
// Written by D. Crocker, Escher Technologies Ltd.
#include "lcd7920.h"

// Pins for serial LCD interface. For speed, we use the SPI interface, which means we need the MOSI and SCLK pins.
// Connection of the 7920-based GLCD:
//  Vss to Gnd
//  Vdd to +5v
//  Vo through 10k variable resistor to +5v (not needed if your board has a contrast adjust pot on the back already, like mine does)
//  RS to +5v
//  PSB to gnd
//  RW to Arduino MOSI (see below for pin mapping)
//  E to Arduino SCLK (see below for pin mapping)
//  RST to +5v
//  D0-D7 unconnected
//  BLK to ground (or collector of NPN transistor if controlling backlight by PWM)
//  BLA via series resistor (if not included in LCD module) to +5v or other supply

// The following pin numbers must not be changed because they must correspond to the processor MOSI and SCLK pins
// We don't actually use the MISO pin, but it gets forced as an input when we enable SPI
// The SS pin MUST be configured as an output (if it is used as an input and goes low, it will kill the communication with the LCD).

const int MOSIpin = A1; // 11
const int MISOpin = A3; // 12
const int SCLKpin = A2; // 13
const int SSpin = A0; // 10

// End of configuration section

// LCD
static Lcd7920 lcd(SCLKpin, MOSIpin, 0);

// Initialization
void setup()
{
  pinMode(SSpin, OUTPUT);   // must do this before we use the lcd in SPI mode
  digitalWrite(SSpin, HIGH);
  lcd.begin(true);          // init lcd in graphics mode
  }

int x = 0;
int y = 0;
void loop()
{
  x = (x + 1) & 0x7F;
  if (x == 0) y++;
  lcd.setPixel(x,y,PixelSet);
  lcd.flush();
}

// End


