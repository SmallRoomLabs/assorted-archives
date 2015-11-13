#include <avr/pgmspace.h>
#include "SSD1306_OLED.h"
#include <SPI.h>
#include <Wire.h>

//------------------------------------------------------------------------------
// Initialization and screen clear command tables
//------------------------------------------------------------------------------
#if SSD1306_128_64 || SSD1306_128_32
static const uint8_t initTable[] PROGMEM = {		// Initialization command sequence
	SSD1306_DISPLAYOFF,          	// 0xAE
	SSD1306_SETDISPLAYCLOCKDIV,  	// 0xD5
	0x80,                        	// the suggested ratio 0x80
	SSD1306_SETMULTIPLEX,        	// 0xA8
#if SSD1306_128_64
	0x3F,
#else
	0x1F,
#endif
	SSD1306_SETDISPLAYOFFSET,    	// 0xD3
	0x0,                         	// no offset
	SSD1306_SETSTARTLINE | 0x0,  	// line #0
	SSD1306_CHARGEPUMP,          	// 0x8D
	0x14,
	SSD1306_MEMORYMODE,          	// 0x20
	0x00,			  	// was: 0x2 page mode
	SSD1306_SEGREMAP | 0x1,
	SSD1306_COMSCANDEC,
	SSD1306_SETCOMPINS,          	// 0xDA
#if SSD1306_128_64
	0x12,
#else
	0x02,
#endif
	SSD1306_SETCONTRAST,         	// 0x81
	0xCF,
	SSD1306_SETPRECHARGE,         // 0xd9
	0xF1,
	SSD1306_SETVCOMDETECT,      	// 0xDB
	0x40,
	SSD1306_DISPLAYALLON_RESUME, 	// 0xA4
	SSD1306_NORMALDISPLAY,       	// 0xA6
	SSD1306_DISPLAYON		//--turn on oled panel
};

#endif

//------------------------------------------------------------------------------
// SPI bus configuration
// Called before each access: SPI enabled, master, 8 MHz, 0 polarity, 0 phase
//------------------------------------------------------------------------------
void configureBus() {
#if !I2C && HW_SPI
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
#endif
}

//------------------------------------------------------------------------------
// SPI and I2C access primitives
// The I2C is optimized for the 32 byte buffer in the Wire library
//------------------------------------------------------------------------------
#if I2C
void SSD1306ElfDisplay::StartTransfer() {
	i2cCount = 0;
}
inline void SSD1306ElfDisplay::SelectCommand() {
	i2cControl = 0x00;
}
inline void SSD1306ElfDisplay::SelectData() {
	i2cControl = 0x40;
}
inline void SSD1306ElfDisplay::WriteData(uint8_t b) {
	if ((i2cCount == 0) || (i2cCount == (BUFFER_LENGTH-1))) {
		i2cCount = 0;
		Wire.endTransmission();
		Wire.beginTransmission(SSD1306_I2C_ADDRESS);
		Wire.write(i2cControl);
	}
	Wire.write(b);
	i2cCount++;
}
void SSD1306ElfDisplay::EndTransfer() {
	if (!i2cIgnoreEnd)
		Wire.endTransmission();
}
void SSD1306ElfDisplay::I2CRestartTransfer() {
	Wire.endTransmission();
	i2cCount = 0;
}

#else // SPI
inline void SSD1306ElfDisplay::StartTransfer() {
	*csport &= ~cspinmask;
}
inline void SSD1306ElfDisplay::SelectCommand() {
	*dcport &= ~dcpinmask;
}
inline void SSD1306ElfDisplay::SelectData() {
	*dcport |= dcpinmask;
}
#if HW_SPI
inline void SSD1306ElfDisplay::WriteData(uint8_t b) {
	SPI.transfer(b);
}
#else // software SPI
inline void SSD1306ElfDisplay::WriteData(uint8_t b) {
	for(uint8_t bit = 0x80; bit; bit >>= 1) {
		*clkport &= ~clkpinmask;
		if(b & bit) *mosiport |=  mosipinmask;
		else        *mosiport &= ~mosipinmask;
		*clkport |=  clkpinmask;
	}
}
#endif
inline void SSD1306ElfDisplay::EndTransfer() {
	*csport |= cspinmask;
}
#endif

//------------------------------------------------------------------------------
// init()
//------------------------------------------------------------------------------
void SSD1306ElfDisplay::init() {


#if I2C
	Wire.begin();
	i2cIgnoreEnd = false;
#ifdef TWBR_INIT_VALUE
	TWBR = TWBR_INIT_VALUE; // set I2C clock rate (assumed prescale)
#endif
#else // SPI
	pinMode(dc_, OUTPUT);
	pinMode(cs_, OUTPUT);
	csport      = portOutputRegister(digitalPinToPort(cs_));
	cspinmask   = digitalPinToBitMask(cs_);
	dcport      = portOutputRegister(digitalPinToPort(dc_));
	dcpinmask   = digitalPinToBitMask(dc_);
	*csport |= cspinmask;		// chip select -- set to idle
#if HW_SPI
	SPI.begin();
	configureBus();
#else // software SPI
	pinMode(data_, OUTPUT);
	pinMode(clk_, OUTPUT);
	clkport     = portOutputRegister(digitalPinToPort(clk_));
	clkpinmask  = digitalPinToBitMask(clk_);
	mosiport    = portOutputRegister(digitalPinToPort(data_));
	mosipinmask = digitalPinToBitMask(data_);
#endif
#endif

// #if RESET_PULSE
// 	pinMode(rst_, OUTPUT);
// 	digitalWrite(rst_, HIGH);
// 	delay(1);
// 	digitalWrite(rst_, LOW);
// 	delay(10);
// 	digitalWrite(rst_, HIGH);
// #endif

	// Send initialization command sequence
	StartTransfer();
	SelectCommand();
	for (uint8_t i=0; i<sizeof(initTable); i++)
		WriteData(pgm_read_byte(initTable+i));
	EndTransfer();
}

void SSD1306ElfDisplay::clear() {
    configureBus();
    StartTransfer();
    SelectCommand();
    WriteData(SSD1306_SETLOWCOLUMN | 0);
    WriteData(SSD1306_SETHIGHCOLUMN | 0);
    WriteData(SSD1306_SETSTARTPAGE | 0);
//    WriteData(0x20);
//    WriteData(0x01);
    EndTransfer();
    #if I2C
    I2CRestartTransfer();
    #endif
    StartTransfer();
    SelectData();
    for (uint16_t n = 0;n < 128/8*64;n++) WriteData(0);
    for (uint16_t n = 0;n < 256;n++) screenBuffer[n] = 0;
    EndTransfer();
}


void SSD1306ElfDisplay::writeBlock(uint8_t x,uint8_t y,uint8_t *b,uint8_t size) {
    configureBus();
    StartTransfer();
    SelectCommand();
    WriteData(SSD1306_SETLOWCOLUMN | (x & 0XF));
    WriteData(SSD1306_SETHIGHCOLUMN | (x >> 4));
    WriteData(SSD1306_SETSTARTPAGE | y);
    EndTransfer();
    #if I2C
    I2CRestartTransfer();
    #endif
    StartTransfer();
    SelectData();
    for (uint8_t i = 0;i < size;i++) {
    	WriteData(*b);
    	WriteData(*b);
    	b++;
    }
    EndTransfer();
}

//------------------------------------------------------------------------------
// sendCommand(c) - send a single byte command
//------------------------------------------------------------------------------
void SSD1306ElfDisplay::sendCommand(uint8_t c) {
	configureBus();
	StartTransfer();
	SelectCommand();
	WriteData(c);
	EndTransfer();
}

//------------------------------------------------------------------------------
// sendData(c) - send a single byte of data; the cursor col is incremented
//
// This can be used to draw primitive pseudo-graphical elements on the screen
//------------------------------------------------------------------------------
void SSD1306ElfDisplay::sendData(uint8_t c) {
	configureBus();
	StartTransfer();
	SelectData();
	WriteData(c);
	EndTransfer();
}

void SSD1306ElfDisplay::refresh(uint8_t *screen) {
	uint8_t hits = 0;
	uint8_t cell = 0;
	uint8_t ledData[8];
	do {
		if (screen[cell] != screenBuffer[cell]) {									// Has the cell changed ?
			hits++;
			uint8_t xBlock = (cell & 7); 											// x Horizontal Block (8 pixel group)
			uint8_t yBlock = (cell >> 3) >> 2;										// y Vertical Block (4 pixel group)
			uint8_t xPos = xBlock * 8 * 2; 											// Horizontal position
			uint8_t yPos = yBlock;													// Vertical position.
			uint8_t bitMask = 0x80;													// Bitmask we are doing.
			uint8_t ledPtr = 0;														// Index into ledData.
			uint8_t *src = screen;													// Where data is coming from.

			while (bitMask != 0) {													// For each bit pattern until done.
				uint8_t topCell = yBlock * 4 * 8+xBlock; 							// offset in screen.
				uint8_t pattern = 0;												// build word up.
				if ((src[topCell] & bitMask) != 0) pattern |= 0x03;
				if ((src[topCell+8] & bitMask) != 0) pattern |= 0x0C;
				if ((src[topCell+16] & bitMask) != 0) pattern |= 0x30;
				if ((src[topCell+24] & bitMask) != 0) pattern |= 0xC0;

				ledData[ledPtr++] = pattern;										// Copy into led data buffer.
				bitMask = bitMask >> 1;												// Next bitmask.
			}

			writeBlock(xPos,yPos,ledData,8);

			uint8_t cellr = yBlock * 4 * 8 + xBlock; 								// Fix up all the others
			screenBuffer[cellr] = screen[cellr];									// so they don't repaint.
			screenBuffer[cellr+8] = screen[cellr+8];
			screenBuffer[cellr+16] = screen[cellr+16];
			screenBuffer[cellr+24] = screen[cellr+24];
		}
		cell++;																		// Go to next cell
	} while (cell != 0);															// Until done whole screen.
	Serial.println(hits);
}

static uint8_t col[15];

void SSD1306ElfDisplay::drawVBar(uint8_t x,uint8_t y,uint8_t show) {
	col[0] = show ? 0xFF : 0x00;
	writeBlock(x,y,col,1);
	writeBlock(x,y+1,col,1);
}

void SSD1306ElfDisplay::drawHBar(uint8_t x,uint8_t y,uint8_t show) {
	for (uint8_t n = 0;n < 15;n++) col[n] = show ? 0x01 : 0x00;
	writeBlock(x+2,y,col,15);
}

static const uint8_t _segmentTable[] = {
	0x3F,					// 0
	0x0C,					// 1
	0x76,					// 2
	0x5E,					// 3
	0x4D,					// 4
	0x5B,					// 5
	0x7B,					// 6
	0x0E,					// 7
	0x7F,					// 8
	0x5F,					// 9
	0x6F,					// A
	0x79,					// B
	0x33,					// C
	0x7C,					// D
	0x73,					// E
	0x63					// F
};

void SSD1306ElfDisplay::displayBinary(uint8_t n) {
	for (uint8_t r = 0;r < 2;r++) {
		for (uint8_t b = 0;b < 8;b++) {
			for (uint8_t c = 0;c < 8;c++) {
				col[c] = 0xFF;
				if ((n & (0x80 >> b)) == 0) col[c] = (r == 0 ? 0x01:0x80);
			}
			col[7] = 0x00;col[0] = col[6] = (r == 0) ? 0xFE:0x7F;
			writeBlock(b*16,r,col,8);
		}
	}
	for (uint8_t c = 0;c < 2;c++) {
		uint8_t x = (c == 0) ? 64 - 40 : 64 + 8;
		uint8_t segment = _segmentTable[((n >> (4-c*4))) & 0x0F];
		drawVBar(x,3,segment & 1);
		drawHBar(x,3,segment & 2);
		drawVBar(x+32,3,segment & 4);
		drawVBar(x+32,5,segment & 8);
		drawHBar(x,7,segment & 16);
		drawVBar(x,5,segment & 32);
		drawHBar(x,5,segment & 64);
	}
}