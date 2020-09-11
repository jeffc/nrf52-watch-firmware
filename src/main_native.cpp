#ifdef NATIVE
#include "graphics.h"
#include "system.h"
#include <mutex>

extern void setup();
extern void loop();
extern void doit();

// there are a lot of awful things we have to do to get the native SDL graphics
// rendering threads to play nice with our interrupt-driven single-core-CPU
// code. The mutex effectively emulates that core, and we lock/unlock the
// display (or the thread updating it).
extern System* sys;
std::mutex runlock;

int main() {
  setup();
  int running = 1;
  
  Graphics* gfx = sys->getGraphics();

  while (running) {
    loop();
    runlock.lock();
    gfx->display();
    runlock.unlock();
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
