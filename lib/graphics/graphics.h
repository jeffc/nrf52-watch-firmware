/*
 * Graphics functions, built on Adafruit_GFX
 */

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#define SCREEN_WIDTH  230
#define SCREEN_HEIGHT 303

#ifdef NATIVE
#include <Adafruit_GFX_dummy_display.h>

class Graphics : public Adafruit_GFX_dummy_display {

#elif EMBEDDED
#include <Adafruit_SharpMem.h>

class Graphics : public Adafruit_SharpMem {

#endif

  public:
    Graphics();

};

#endif
