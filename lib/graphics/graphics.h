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

  public:
    // stub out methods that exist in Adafruit_Sharpmem but not native
    void clearBuffer();
    void refresh() {display();};
    void printf(const char* st, ...);

#elif EMBEDDED
#include <adafruit_sharpmem.h>

class Graphics : public Adafruit_SharpMem {

#endif

  public:
    Graphics();

};

#ifdef NATIVE
// include this as a faux-singleton to refresh the screen more quickly than we
// otherwise would.
static Graphics* NATIVE_GFX_HACK;
#endif

#endif
