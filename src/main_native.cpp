#ifdef NATIVE
#include <map>

#include "graphics/graphics.h"
#include "system/system.h"
#include "pins.h"

// these are declared in main.cpp
extern void setup();
extern void loop();
extern void doit();
extern System *sys;

// convert SDL keypresses into interrupts for the interrupts engine
std::map<SDL_Keycode, int> keys_map = {
  {SDLK_UP, PIN_BUTTON1},
  {SDLK_DOWN, PIN_BUTTON3},
  {SDLK_LEFT, PIN_BUTTON5},
};

void handleKeyEvent(SDL_Keycode key, bool keydown) {
  auto it = keys_map.find(key);

  if (key == SDLK_t) {
    printf("boop\n");
    sys->refreshDisplay();
    sys->getGraphics()->display();
  }

  if (it != keys_map.end()) {
    if (keydown) {
      System::buttonsPressed.insert(it->second);
    } else {
      System::buttonsPressed.erase(it->second);
    }
    sys->fireIRQ(it->second, (keydown) ? FALLING : RISING);
    sys->fireIRQ(it->second, CHANGE);

  }
}


uint32_t oneSecondCallbackFn(uint32_t interval, void *param) {
  sys->fireIRQ(PIN_SQW, RISING);
  //sys->getGraphics()->display();
  //sys->refreshDisplay();
  return 1000; // set the next timer for 1s from now
}

int main() {
  setup();
  int running = 1;

  SDL_AddTimer(1000, oneSecondCallbackFn, NULL);
  Graphics *gfx = sys->getGraphics();

  while (running) {
    loop();
    SDL_Event event;
    if (SDL_PollEvent(&event) == 1) {
      switch (event.type) {
      case SDL_QUIT:
        running = 0;
        break;
      case SDL_KEYDOWN:
        if (event.key.repeat == 0) {
          handleKeyEvent(event.key.keysym.sym, true); 
        }
        break;
      case SDL_KEYUP:
        handleKeyEvent(event.key.keysym.sym, false); 
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
