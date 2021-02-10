#ifdef EMBEDDED

#include "pins.h"
#include <graphics.h>

Graphics::Graphics()
    : Memory_LCD_SPI(PIN_LCD_CS) {
  begin();
  setRotation(2);
}

#endif
