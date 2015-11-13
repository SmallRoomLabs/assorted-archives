#include <Arduino.h>
#include <Print.h>

// Class for driving 128x64 graphical LCD fitted wirth ST7920 controller
// This drives the GLCD in serial mode so that it needs just 2 pins.
// Preferably, we use SPI to do the comms.

// Derive the LCD class from the Print class so that we can print stuff to it in alpha mode
class WP1Driver : public Print
{
public:
  WP1Driver(uint8_t cPin, uint8_t dPin, bool useSpi);    // constructor
 
  //  keeps the compiler happy.
  virtual size_t write(uint8_t c);                 // write a character
  void begin();
  void clear();
  void setPixel(uint8_t x,uint8_t y,uint8_t isOn); 
  void repaint();
  void writeHexDigit(uint8_t x,uint8_t y,uint8_t b);
  void writeBinary(uint8_t x,uint8_t y,uint8_t b);
private:
  bool gfxMode;
  bool extendedMode;
  bool useSpi;
  uint8_t clockPin, dataPin;

  uint8_t pixelMemory[4*32];                      // 32 x 32 pixel memory (8 bits x 4 columns x 32 rows)
  uint8_t dirtyFlags[4*32/8];                     // Flag for being 'dirty'.
  uint8_t needsRepaint;                           // Non zero when something needs repainting e.g. is dirty.

  void sendLcdCommand(uint8_t command);
  void sendLcdData(uint8_t data);
  void sendLcd(uint8_t data1, uint8_t data2);
  void sendLcdSlow(uint8_t data);
  void commandDelay();
  void ensureBasicMode();
  void ensureExtendedMode();
  void updateScreen(uint8_t x,uint8_t y);
  void writeBlock(uint8_t x,uint8_t y,uint8_t block);
};
