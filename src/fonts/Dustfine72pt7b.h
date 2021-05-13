#include <Arduino.h>
#include <Adafruit_GFX.h>
extern const uint8_t Dustfine72pt7bBitmaps[];

extern const GFXglyph Dustfine72pt7bGlyphs[];
const GFXfont Dustfine72pt7b PROGMEM = {(uint8_t *)Dustfine72pt7bBitmaps,
                                        (GFXglyph *)Dustfine72pt7bGlyphs, 0x20,
                                        0x7E, 162};

// Approx. 65375 bytes
