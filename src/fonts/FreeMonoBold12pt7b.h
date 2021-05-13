#include <Arduino.h>
#include <Adafruit_GFX.h>
extern const uint8_t FreeMonoBold12pt7bBitmaps[];
extern const GFXglyph FreeMonoBold12pt7bGlyphs[];
const GFXfont FreeMonoBold12pt7b PROGMEM = {
    (uint8_t *)FreeMonoBold12pt7bBitmaps, (GFXglyph *)FreeMonoBold12pt7bGlyphs,
    0x20, 0x7E, 24};

// Approx. 2402 bytes
