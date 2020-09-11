#ifdef NATIVE

#include <stdio.h>
#include <stdint.h>
#include "pins.h"
#include "system.h"
#include <Adafruit_GFX_dummy_display.h>
#include <mutex>

extern std::mutex runlock;

System::System() {
  _gfx = new Graphics();
  _rtc = new RTC();
  _battery = new Battery();
}

uint32_t oneSecondCallbackFn(uint32_t interval, void* param) {
  runlock.lock();
  ((void (*)())param)(); // call param as a function pointer
  runlock.unlock();
  return 1000; // set the next timer for 1s from now
}

void System::registerIRQ(int pinnum, void (*fn)(), int mode) {
  // special case the ones we know about
  if (pinnum == PIN_SQW && mode == RISING) {
    printf("Registering SQW timer\n");
    SDL_AddTimer(1000, oneSecondCallbackFn, (void*)fn);
  } else {
    printf("Tried to register unknown IRQ %d (%d)\n", pinnum, mode);
  }
}

void System::feedWatchdog() {
}

Graphics* System::getGraphics() {
  return _gfx;
}

RTC* System::getRTC() {
  return _rtc;
}

Battery* System::getBattery() {
  return _battery;
}

#endif
