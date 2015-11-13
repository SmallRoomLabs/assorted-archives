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
void SSD1306Simple::StartTransfer() {
	i2cCount = 0;
}
inline void SSD1306Simple::SelectCommand() {
	i2cControl = 0x00;
}
inline void SSD1306Simple::SelectData() {
	i2cControl = 0x40;
}
inline void SSD1306Simple::WriteData(uint8_t b) {
	if ((i2cCount == 0) || (i2cCount == (BUFFER_LENGTH-1))) {
		i2cCount = 0;
		Wire.endTransmission();
		Wire.beginTransmission(SSD1306_I2C_ADDRESS);
		Wire.write(i2cControl);
	}
	Wire.write(b);
	i2cCount++;
}
void SSD1306Simple::EndTransfer() {
	if (!i2cIgnoreEnd)
		Wire.endTransmission();
}
void SSD1306Simple::I2CRestartTransfer() {
	Wire.endTransmission();
	i2cCount = 0;
}

#else // SPI
inline void SSD1306Simple::StartTransfer() {
	*csport &= ~cspinmask;
}
inline void SSD1306Simple::SelectCommand() {
	*dcport &= ~dcpinmask;
}
inline void SSD1306Simple::SelectData() {
	*dcport |= dcpinmask;
}
#if HW_SPI
inline void SSD1306Simple::WriteData(uint8_t b) {
	SPI.transfer(b);
}
#else // software SPI
inline void SSD1306Simple::WriteData(uint8_t b) {
	for(uint8_t bit = 0x80; bit; bit >>= 1) {
		*clkport &= ~clkpinmask;
		if(b & bit) *mosiport |=  mosipinmask;
		else        *mosiport &= ~mosipinmask;
		*clkport |=  clkpinmask;
	}
}
#endif
inline void SSD1306Simple::EndTransfer() {
	*csport |= cspinmask;
}
#endif

//------------------------------------------------------------------------------
// init()
//------------------------------------------------------------------------------
void SSD1306Simple::init() {


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

void SSD1306Simple::clear() {
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
    EndTransfer();
}


#include "font.h"

void SSD1306Simple::loadRow(uint8_t x,uint8_t y,uint8_t ch) {
    configureBus();
    StartTransfer();
    SelectCommand();
    WriteData(SSD1306_SETLOWCOLUMN | (x & 0XF));
    WriteData(SSD1306_SETHIGHCOLUMN | (x >> 4));
    WriteData(SSD1306_SETSTARTPAGE | y);
//    WriteData(0x20);
//    WriteData(0x01);
    EndTransfer();
    #if I2C
    I2CRestartTransfer();
    #endif
    StartTransfer();
    SelectData();
    uint16_t fontOffset = (ch - ' ') *5;
    for (uint8_t n = 0;n < 5;n++) {
        WriteData(pgm_read_byte(fontdata+fontOffset+n));
    }
    EndTransfer();
}

void SSD1306Simple::drawCharacter(uint8_t x,uint8_t y,uint8_t ch) {
	if (x < 0 || y < 0 || x > 128-5 || y > 8) return;
	if (ch >= ' ' & ch <= 0x7F) {
		loadRow(x,y,ch);
	}
}
//------------------------------------------------------------------------------
// sendCommand(c) - send a single byte command
//------------------------------------------------------------------------------
void SSD1306Simple::sendCommand(uint8_t c) {
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
void SSD1306Simple::sendData(uint8_t c) {
	configureBus();
	StartTransfer();
	SelectData();
	WriteData(c);
	EndTransfer();
}
