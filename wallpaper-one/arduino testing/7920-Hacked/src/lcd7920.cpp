// Driver for 128x634 graphical LCD with ST7920 controller
// D Crocker, Escher Technologies Ltd.

#include "lcd7920.h"
#include <pins_arduino.h>
#include <avr/interrupt.h>

// LCD basic instructions. These all take 72us to execute except LcdDisplayClear, which takes 1.6ms
const uint8_t LcdDisplayClear = 0x01;
const uint8_t LcdHome = 0x02;
const uint8_t LcdEntryModeSet = 0x06;       // move cursor right and indcement address when writing data
const uint8_t LcdDisplayOff = 0x08;
const uint8_t LcdDisplayOn = 0x0C;          // add 0x02 for cursor on and/or 0x01 for cursor blink on
const uint8_t LcdFunctionSetBasicAlpha = 0x20;
const uint8_t LcdFunctionSetBasicGraphic = 0x22;
const uint8_t LcdFunctionSetExtendedAlpha = 0x24;
const uint8_t LcdFunctionSetExtendedGraphic = 0x26;
const uint8_t LcdSetDdramAddress = 0x80;    // add the address we want to set

// LCD extended instructions
const uint8_t LcdSetGdramAddress = 0x80;

const unsigned int LcdCommandDelayMicros = 72 - 24; // 72us required, less 24us time to send the command @ 1MHz
const unsigned int LcdDataDelayMicros = 10;         // Delay between sending data bytes
const unsigned int LcdDisplayClearDelayMillis = 3;  // 1.6ms should be enough

const unsigned int numRows = 64;
const unsigned int numCols = 128;

Lcd7920::Lcd7920(uint8_t cPin, uint8_t dPin, bool spi) : clockPin(cPin), dataPin(dPin), useSpi(spi)
{
}

size_t Lcd7920::write(uint8_t ch)
{
  return 1;
}

// NB - if using SPI then the SS pin must be set to be an output before calling this!
void Lcd7920::begin(bool gmode)
{
  
  // Set up the SPI interface for talking to the LCD. We have to set MOSI, SCLK and SS to outputs, then enable SPI.
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  if (useSpi)
  {
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // enable SPI, master mode, clock low when idle, data sampled on rising edge, clock = f/16 (= 1MHz), send MSB first
    //SPSR = (1 << SPI2X);  // double the speed to 2MHz (optional)
  }

  gfxMode = false;
  sendLcdCommand(LcdFunctionSetBasicAlpha);
  delay(1);
  sendLcdCommand(LcdFunctionSetBasicAlpha);
  commandDelay();
  sendLcdCommand(LcdEntryModeSet);
  commandDelay();
  extendedMode = false;
  clear();
    gfxMode = true;
    clear();  // clear gfx ram
  sendLcdCommand(LcdDisplayOn);
  commandDelay();
}

void Lcd7920::clear()
{
  if (gfxMode)
  {
    memset(image, 0, sizeof(image));
    // flag whole image as dirty and update
    startRow = 0;
    endRow = numRows;
    startCol = 0;
    endCol = numCols;
    flush();
  }
  else
  {
    ensureBasicMode();
    sendLcdCommand(LcdDisplayClear);
    delay(LcdDisplayClearDelayMillis);
  }
}

// Flush the dirty part of the image to the lcd
void Lcd7920::flush()
{
  if (gfxMode && endCol > startCol && endRow > startRow)
  {
    uint8_t startColNum = startCol/16;
    uint8_t endColNum = (endCol + 15)/16;
    for (uint8_t r = startRow; r < endRow; ++r)
    {
      setGraphicsAddress(r, startColNum);
      uint8_t *ptr = image + ((16 * r) + (2 * startColNum));
      for (uint8_t i = startColNum; i < endColNum; ++i)
      {      
        sendLcdData(*ptr++);
        //commandDelay();    // don't seem to need a delay here
        sendLcdData(*ptr++);
        //commandDelay();    // don't seem to need as long a delay as this
        delayMicroseconds(LcdDataDelayMicros);
      }
    }
    startRow = numRows;
    startCol = numCols;
    endCol = endRow = 0;
  }
}

void Lcd7920::setPixel(uint8_t x, uint8_t y, PixelMode mode)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    uint8_t mask = 0x80u >> (x%8);
    switch(mode)
    {
      case PixelClear:
        *p &= ~mask;
        break;
      case PixelSet:
        *p |= mask;
        break;
      case PixelFlip:
        *p ^= mask;
        break;
    }
    
    // Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
    if (startRow > y) { startRow = y; }
    if (endRow <= y)  { endRow = y + 1; }
    if (startCol > x) { startCol = x; }
    if (endCol <= x)  { endCol = x + 1; }
  }
}


void Lcd7920::setGraphicsAddress(unsigned int r, unsigned int c)
{
  if (gfxMode)
  {
    ensureExtendedMode();
    sendLcdCommand(LcdSetGdramAddress | (r & 31));
    //commandDelay();  // don't seem to need this one
    sendLcdCommand(LcdSetGdramAddress | c | ((r & 32) >> 2));
    commandDelay();    // we definitely need this one
  }
}

void Lcd7920::commandDelay()
{
  delayMicroseconds(LcdCommandDelayMicros);
}

// Send a command to the LCD
void Lcd7920::sendLcdCommand(uint8_t command)
{
  sendLcd(0xF8, command);
}

// Send a data byte to the LCD
void Lcd7920::sendLcdData(uint8_t data)
{
  sendLcd(0xFA, data);
}

// Send a command to the lcd. Data1 is sent as-is, data2 is split into 2 bytes, high nibble first.
void Lcd7920::sendLcd(uint8_t data1, uint8_t data2)
{
  if (useSpi)
  {
    SPDR = data1;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 & 0xF0;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 << 4;
    while ((SPSR & (1 << SPIF)) == 0) { }
  }
  else
  {
    sendLcdSlow(data1);
    sendLcdSlow(data2 & 0xF0);
    sendLcdSlow(data2 << 4);
  }
}

void Lcd7920::sendLcdSlow(uint8_t data)
{
#if 1

  // Fast shiftOut function
  volatile uint8_t *sclkPort = portOutputRegister(digitalPinToPort(clockPin));
  volatile uint8_t *mosiPort = portOutputRegister(digitalPinToPort(dataPin));
  uint8_t sclkMask = digitalPinToBitMask(clockPin);
  uint8_t mosiMask = digitalPinToBitMask(dataPin);
  
  uint8_t oldSREG = SREG;
  cli();
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (data & 0x80)
    {
      *mosiPort |= mosiMask;
    }
    else
    {
      *mosiPort &= ~mosiMask;
    }
    *sclkPort |= sclkMask;
    *sclkPort &= ~sclkMask;
    data <<= 1;
  }
  SREG = oldSREG;
  
 #else
 
  // really slow version, like Arduino shiftOut function 
  for (uint8_t i = 0; i < 8; ++i)
  {
    // The following could be made much faster using direct port manipulation, however we normally use SPI anyway.
    // We could use the Arduino shiftOut function, bit that is even slower than this.
    digitalWrite(dataPin, (data & 0x80) ? HIGH : LOW);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    data <<= 1;
  }
  
#endif  
}

void Lcd7920::ensureBasicMode()
{
  if (extendedMode)
  {
    sendLcdCommand(gfxMode ? LcdFunctionSetBasicGraphic : LcdFunctionSetBasicAlpha);
    commandDelay();
    extendedMode = false;
  }
}

void Lcd7920::ensureExtendedMode()
{
  if (!extendedMode)
  {
    sendLcdCommand(gfxMode ? LcdFunctionSetExtendedGraphic : LcdFunctionSetExtendedAlpha);
    commandDelay();
    extendedMode = true;
  }
}
