#include <Arduino.h>
#include <Print.h>

// Enumeration for specifying drawing modes
enum PixelMode
{
  PixelClear = 0,    // clear the pixel(s)
  PixelSet = 1,      // set the pixel(s)
  PixelFlip = 2      // invert the pixel(s)
};

// Class for driving 128x64 graphical LCD fitted wirth ST7920 controller
// This drives the GLCD in serial mode so that it needs just 2 pins.
// Preferably, we use SPI to do the comms.

// Derive the LCD class from the Print class so that we can print stuff to it in alpha mode
class Lcd7920 : public Print
{
public:
  // Construct a GLCD driver.
  //  cPin = clock pin (connects to E pin of ST7920)
  //  dPin = data pin (connects to R/W pin of ST7920)
  // useSpi = true to use hardware SPI. If true then cPin must correspond to SCLK, dPin must correspond to MOSI, and SS must be configured as an output.
  Lcd7920(uint8_t cPin, uint8_t dPin, bool useSpi);    // constructor
  
  //  keeps the compiler happy.
  virtual size_t write(uint8_t c);                 // write a character

  // Initialize the display. Call this in setup(). If using graphics mode, also call setFont to select initial text font.
  //  gmode = true to use graphics mode, false to use ST7920 alphanumeric mode.
  void begin(bool gmode);
  
  // Clear the display. Works in both graphic and alphanumeric mode. Also selects non-inverted text.
  void clear();
  
 
  // Flush the display buffer to the display. In graphics mode, calls to write, setPixel, line and circle will not be committed to the display until this is called.
  void flush();
  
  // Set, clear or invert a pixel
  //  x = x-coordinate of the pixel, measured from left hand edge of the display
  //  y = y-coordinate of the pixel, measured down from the top of the display
  //  mode = whether we want to set, clear or invert the pixel
  void setPixel(uint8_t x, uint8_t y, PixelMode mode);
    
private:
  bool gfxMode;
  bool extendedMode;
  bool useSpi;
  uint8_t clockPin, dataPin;
  uint16_t lastCharColData;                   // data for the last non-space column, used for kerning
  uint8_t row, column;
  uint8_t startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
  uint8_t image[(128 * 64)/8];                // image buffer, 1K in size (= half the RAM of the Uno)
  
  void sendLcdCommand(uint8_t command);
  void sendLcdData(uint8_t data);
  void sendLcd(uint8_t data1, uint8_t data2);
  void sendLcdSlow(uint8_t data);
  void commandDelay();
  void setGraphicsAddress(unsigned int r, unsigned int c);
  void ensureBasicMode();
  void ensureExtendedMode();
};
