/***
 * Hacked up version of the adafruit sharp memory LCD library. Modified to use
 * hardware SPI to update the display, which is approximately 4x faster (39ms
 * instead of ~170ms)
 * 
 * Values for screen height and width are hard-coded in a few places.
 *
 * Original adafruit text follows, per their license:
 */
/*********************************************************************
This is an Arduino library for our Monochrome SHARP Memory Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1393

These displays use SPI to communicate, 3 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifdef EMBEDDED

#include "memory_lcd_spi.h"
#include <SPI.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif
#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b)                                                   \
  {                                                                            \
    uint16_t t = a;                                                            \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

/**************************************************************************
    Sharp Memory Display Connector
    -----------------------------------------------------------------------
    Pin   Function        Notes
    ===   ==============  ===============================
      1   VIN             3.3-5.0V (into LDO supply)
      2   3V3             3.3V out
      3   GND
      4   SCLK            Serial Clock
      5   MOSI            Serial Data Input
      6   CS              Serial Chip Select
      9   EXTMODE         COM Inversion Select (Low = SW clock/serial)
      7   EXTCOMIN        External COM Inversion Signal
      8   DISP            Display On(High)/Off(Low)

 **************************************************************************/

#define SHARPMEM_BIT_VCOM (0x40)
#define SHARPMEM_BIT_CLEAR (0x20)

// LSB First
byte *spi_buffer;
#define BUFFER_LEN_BYTES (((16+240)*303 + 16) / 8 + 2)

void setupBuffer(uint8_t* buf) {
  memset(buf, 0xFF, BUFFER_LEN_BYTES);

  for (int row = 0; row < 303; row++) {
    buf[row*32 + 1] = ((row+1) >> 1) & 0xFF;
    buf[row*32] = ((row+1) % 2) ? 0x80 : 0x00;
  }

  buf[0] = 0b0000001; // mode bits
}

/* ************* */
/* CONSTRUCTORS  */
/* ************* */
Memory_LCD_SPI::Memory_LCD_SPI(uint8_t ss)
    : Adafruit_GFX(230, 303) {
  _ss = ss;
}

boolean Memory_LCD_SPI::begin(void) {
  // Set pin state before direction to make sure they start this way (no
  // glitching)
  digitalWrite(_ss, HIGH);
  digitalWrite(PIN_SPI_SCK, LOW);
  digitalWrite(PIN_SPI_MOSI, HIGH);

  pinMode(_ss, OUTPUT);
  pinMode(PIN_SPI_SCK, OUTPUT);
  pinMode(PIN_SPI_MOSI, OUTPUT);

#if defined(USE_FAST_PINIO)
  clkport = portOutputRegister(digitalPinToPort(PIN_SPI_SCK));
  clkpinmask = digitalPinToBitMask(PIN_SPI_SCK);
  dataport = portOutputRegister(digitalPinToPort(PIN_SPI_MOSI));
  datapinmask = digitalPinToBitMask(PIN_SPI_MOSI);
#endif

  // Set the vcom bit to a defined state
  _sharpmem_vcom = SHARPMEM_BIT_VCOM;

  spi_buffer = (uint8_t*) malloc(((16+240)*303+16)/8);
  setupBuffer(spi_buffer);

  setRotation(0);

  return true;
}

/* ************** */
/* PUBLIC METHODS */
/* ************** */

// 1<<n is a costly operation on AVR -- table usu. smaller & faster
static const uint8_t PROGMEM set[] = {1, 2, 4, 8, 16, 32, 64, 128},
                             clr[] = {(uint8_t)~1,  (uint8_t)~2,  (uint8_t)~4,
                                      (uint8_t)~8,  (uint8_t)~16, (uint8_t)~32,
                                      (uint8_t)~64, (uint8_t)~128};

/**************************************************************************/
/*!
    @brief Draws a single pixel in image buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)
*/
/**************************************************************************/
void Memory_LCD_SPI::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return;

  switch (rotation) {
  case 1:
    _swap_int16_t(x, y);
    x = WIDTH - 1 - x;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    _swap_int16_t(x, y);
    y = HEIGHT - 1 - y;
    break;
  }

  if (color) {
    spi_buffer[y*32 + 2 + x/8] |= pgm_read_byte(&set[x%8]);
  } else {
    spi_buffer[y*32 + 2 + x/8] &= pgm_read_byte(&clr[x%8]);
  }
}

/**************************************************************************/
/*!
    @brief Gets the value (1 or 0) of the specified pixel from the buffer

    @param[in]  x
                The x position (0 based)
    @param[in]  y
                The y position (0 based)

    @return     1 if the pixel is enabled, 0 if disabled
*/
/**************************************************************************/
uint8_t Memory_LCD_SPI::getPixel(uint16_t x, uint16_t y) {
  if ((x >= _width) || (y >= _height))
    return 0; // <0 test not needed, unsigned

  switch (rotation) {
  case 1:
    _swap_uint16_t(x, y);
    x = WIDTH - 1 - x;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    _swap_uint16_t(x, y);
    y = HEIGHT - 1 - y;
    break;
  }

  return spi_buffer[y*32 + 2 + x/8] & pgm_read_byte(&set[x%8]);
}

/**************************************************************************/
/*!
    @brief Clears the buffer, but doesn't send anything to the screen
*/
/**************************************************************************/
void Memory_LCD_SPI::clearBuffer() {
  setupBuffer(spi_buffer);
}

/**************************************************************************/
/*!
    @brief Clears the screen
*/
/**************************************************************************/
void Memory_LCD_SPI::clearDisplay() {
  setupBuffer(spi_buffer);
  // Send the clear screen command rather than doing a HW refresh (quicker)
  digitalWrite(_ss, HIGH);
  uint8_t cls[] = {SHARPMEM_BIT_CLEAR, 0x00};
  SPI.beginTransaction(SPISettings(2e6, LSBFIRST, SPI_MODE0));
  SPI.transfer(cls, NULL, 2);
  SPI.endTransaction();
  digitalWrite(_ss, LOW);
}

/**************************************************************************/
/*!
    @brief Renders the contents of the pixel buffer on the LCD
*/
/**************************************************************************/
void Memory_LCD_SPI::refresh(void) {
  digitalWrite(_ss, HIGH);
  SPI.beginTransaction(SPISettings(2e6, LSBFIRST, SPI_MODE0));
  SPI.transfer(spi_buffer, NULL, BUFFER_LEN_BYTES);
  SPI.endTransaction();
  digitalWrite(_ss, LOW);


}
#endif
