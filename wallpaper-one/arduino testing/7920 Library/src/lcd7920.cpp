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

static uint8_t map4to8[16];                   // Maps 4 pixels 2 8 large pixels.

WP1Driver::WP1Driver(uint8_t cPin, uint8_t dPin, bool spi) : clockPin(cPin), dataPin(dPin), useSpi(spi) {
    for (uint8_t i = 0; i < 16; i++) {
        map4to8[i] = 0;
        if ((i & 1) != 0) map4to8[i] |= 0x03;
        if ((i & 2) != 0) map4to8[i] |= 0x0C;
        if ((i & 4) != 0) map4to8[i] |= 0x30;
        if ((i & 8) != 0) map4to8[i] |= 0xC0;
    }
}

size_t WP1Driver::write(uint8_t ch) {
    return 1;
}

// NB - if using SPI then the SS pin must be set to be an output before calling this!
void WP1Driver::begin() {

    // Set up the SPI interface for talking to the LCD. We have to set MOSI, SCLK and SS to outputs, then enable SPI.
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, LOW);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    if (useSpi) {
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

void WP1Driver::clear() {
    if (gfxMode) {
        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 64; y++) {
                writeBlock(x,y,0);
            }
        }
        for (int i = 0; i < 4*32; i++) pixelMemory[i] = 0;
        for (int i = 0; i < 4*32/8; i++) dirtyFlags[i] = 0;
        needsRepaint = 0;
    } else {
        ensureBasicMode();
        sendLcdCommand(LcdDisplayClear);
        delay(LcdDisplayClearDelayMillis);
    }
}

void WP1Driver::writeBlock(uint8_t x,uint8_t y,uint8_t block) {
    ensureExtendedMode();
    uint8_t b1 = map4to8[block >> 4];
    uint8_t b2 = map4to8[block & 0x0F];
    for (uint8_t r = y; r < y+2; r++) {
        sendLcdCommand(LcdSetGdramAddress | (r & 0x1F));
        sendLcdCommand(LcdSetGdramAddress | (x | ((r & 0x20) >> 2)));
        commandDelay();
        sendLcdData(b1);
        sendLcdData(b2);
        delayMicroseconds(LcdDataDelayMicros);
    }
}

void WP1Driver::commandDelay() {
    delayMicroseconds(LcdCommandDelayMicros);
}

// Send a command to the LCD
void WP1Driver::sendLcdCommand(uint8_t command) {
    sendLcd(0xF8, command);
}

// Send a data byte to the LCD
void WP1Driver::sendLcdData(uint8_t data) {
    sendLcd(0xFA, data);
}

// Send a command to the lcd. Data1 is sent as-is, data2 is split into 2 bytes, high nibble first.
void WP1Driver::sendLcd(uint8_t data1, uint8_t data2) {
    if (useSpi) {
        SPDR = data1;
        while ((SPSR & (1 << SPIF)) == 0) { }
        SPDR = data2 & 0xF0;
        while ((SPSR & (1 << SPIF)) == 0) { }
        SPDR = data2 << 4;
        while ((SPSR & (1 << SPIF)) == 0) { }
    } else {
        sendLcdSlow(data1);
        sendLcdSlow(data2 & 0xF0);
        sendLcdSlow(data2 << 4);
    }
}

void WP1Driver::sendLcdSlow(uint8_t data) {
#if 1

    // Fast shiftOut function
    volatile uint8_t *sclkPort = portOutputRegister(digitalPinToPort(clockPin));
    volatile uint8_t *mosiPort = portOutputRegister(digitalPinToPort(dataPin));
    uint8_t sclkMask = digitalPinToBitMask(clockPin);
    uint8_t mosiMask = digitalPinToBitMask(dataPin);

    uint8_t oldSREG = SREG;
    cli();
    for (uint8_t i = 0; i < 8; ++i) {
        if (data & 0x80) {
            *mosiPort |= mosiMask;
        } else {
            *mosiPort &= ~mosiMask;
        }
        *sclkPort |= sclkMask;
        *sclkPort &= ~sclkMask;
        data <<= 1;
    }
    SREG = oldSREG;

#else

    // really slow version, like Arduino shiftOut function
    for (uint8_t i = 0; i < 8; ++i) {
        // The following could be made much faster using direct port manipulation, however we normally use SPI anyway.
        // We could use the Arduino shiftOut function, bit that is even slower than this.
        digitalWrite(dataPin, (data & 0x80) ? HIGH : LOW);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
        data <<= 1;
    }

#endif
}

void WP1Driver::ensureBasicMode() {
    if (extendedMode) {
        sendLcdCommand(gfxMode ? LcdFunctionSetBasicGraphic : LcdFunctionSetBasicAlpha);
        commandDelay();
        extendedMode = false;
    }
}

void WP1Driver::ensureExtendedMode() {
    if (!extendedMode) {
        sendLcdCommand(gfxMode ? LcdFunctionSetExtendedGraphic : LcdFunctionSetExtendedAlpha);
        commandDelay();
        extendedMode = true;
    }
}

void WP1Driver::setPixel(uint8_t x,uint8_t y,uint8_t isOn) {
    if (((x | y) & 0xE0) != 0) return;                                      // Range check both 0-31.
    uint8_t index = (x / 8) * 32 + y;                                       // Index into pixel array
    uint8_t pixel = 0x80 >> (x % 8);                                        // Bit to set.
    uint8_t mask = pixel ^ 0xFF;                                            // Mask for pixel.
    if ((pixelMemory[index] & mask) != pixel) {                             // Have we changed ?
        if (isOn == 0) pixel = 0;                                           // May be clearing pixel.
        pixelMemory[index] = (pixelMemory[index] & mask) | pixel;           // Update it.   
        dirtyFlags[index / 8] |= (0x80 >> (index % 8));                     // Set the dirty flag for that byte.
        needsRepaint = -1;                                                  // Mark as needs repainting.
        //updateScreen(x,y);
    }
}

void WP1Driver::updateScreen(uint8_t x,uint8_t y) {
    uint8_t index = (x / 8) * 32 + y;                                       // Index into pixel array
    writeBlock(x/8,y*2,pixelMemory[index]);                                 // Display it.
}

void WP1Driver::repaint() {
    if (needsRepaint == 0) return;                                          // Nothing to repaint.
    for (uint8_t row = 0;row < 4*32/8;row++) {    
      if (dirtyFlags[row] != 0) {                                           // Is something in that row dirty ?
        for (uint8_t bit = 0;bit < 8;bit++) {                               // Work through each bit.
          if ((dirtyFlags[row] & (0x80 >> bit)) != 0) {                     // Is the dirty bit set ?
            uint8_t index = row * 8 + bit;                                  // This is the index.
            updateScreen((index >> 5)*8,index & 31);                        // update it.
          }
        }
        dirtyFlags[row] = 0;                                                // Clear the dirty flag.
      }
    }
    needsRepaint = 0;                                                       // Done repainting.
}

static const uint8_t binaryTo7Segment[16] =                                 // Binary to 7 segment table.
  { 1+2+4+8+16+32,2+4,1+2+8+16+64,1+2+4+8+64,2+4+32+64,1+4+8+32+64,1+4+8+16+32+64,1+2+4,1+2+4+8+16+32+64,1+2+4+32+64,
    1+2+4+16+32+64,4+8+16+32+64,1+8+16+32,2+4+8+16+64,1+8+16+32+64,1+16+32+64 };

#define LINE(p)  { writeBlock(x,y,p);y = y + 2; }
#define SEL(m,a1,a2) ((b & (m)) != 0 ? (a1):(a2))

void WP1Driver::writeHexDigit(uint8_t x,uint8_t y,uint8_t b) {
  b = binaryTo7Segment[b & 0xF];                                            // Convert to 7 segment pattern.
  LINE(SEL(1,0x7F,0x00));                                                   // Segment A
  for(int i = 0;i < 3;i++) LINE(SEL(0x20,0x40,0x00)|SEL(0x02,0x01,0x00));   // Segments B,F
  LINE(SEL(0x40,0x7F,0x00));                                                // Segmnent g
  for(int i = 0;i < 3;i++) LINE(SEL(0x10,0x40,0x00)|SEL(0x04,0x01,0x00));   // Segments C,E
  LINE(SEL(8,0x7F,0x00));                                                   // Segment D
}

void WP1Driver::writeBinary(uint8_t x,uint8_t y,uint8_t b) {
  for (uint8_t i = 0;i < 4;i++) {
    uint8_t bw = 0;
    if (b & 0x80) bw |= 0x70;                                               // 2 per column
    if (b & 0x40) bw |= 0x07;
    writeBlock(x,y,bw);                                                     // Output
    writeBlock(x,y+2,bw);
    writeBlock(x,y+4,bw);
    writeBlock(x,y+6,bw);
    writeBlock(x,y+8,0x77);                                                 // With a bar at the bottom.
    b = b << 2;
    x++;
  }
}
