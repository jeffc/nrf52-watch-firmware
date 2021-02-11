#ifdef NATIVE

#include <graphics.h>
#include <stdio.h>

Graphics::Graphics()
    : Adafruit_GFX_dummy_display(SCREEN_WIDTH, SCREEN_HEIGHT, 3 /* zoom */) {}

void Graphics::clearBuffer() { clearDisplay(); }

void Graphics::printf(const char *st, ...) {
  char txt[1024] = {'\0'};
  va_list args;
  va_start(args, st);
  vsnprintf(txt, 1023, st, args);
  va_end(args);
  this->print(txt);
}

#endif
