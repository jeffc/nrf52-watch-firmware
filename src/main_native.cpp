#ifdef NATIVE
#include "graphics.h"

extern void setup();
extern void loop();

int main() {
  setup();
  int running = 1;

  while (running) {
    loop();
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
