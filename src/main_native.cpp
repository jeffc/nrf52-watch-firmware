#ifdef NATIVE
#include "graphics.h"

extern void setup();
extern void loop();
extern void doit();

int main() {
  setup();
  int running = 1;

  while (running) {
    loop();
    if(NATIVE_GFX_HACK) {
      NATIVE_GFX_HACK->refresh();
    }
    SDL_Event event;
    if ( SDL_PollEvent(&event) == 1 )
    {
      switch (event.type)
      {
        case SDL_QUIT:
          running = 0;
          break;
        default:
          /* running
           * */
          break;
      }
    }
  }
}

#endif
