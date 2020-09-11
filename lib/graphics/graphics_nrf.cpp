#ifdef EMBEDDED

#include <graphics.h>
#include "pins.h"

Graphics::Graphics() : Adafruit_SharpMem(
    PIN_SPI_SCK,
    PIN_SPI_MOSI,
    PIN_LCD_CS,
    SCREEN_WIDTH, SCREEN_HEIGHT, 10, 0) { // +10 for dummy pixels
  begin();
  setRotation(2);
}

#endif
