#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <battery.h>
#include <graphics.h>
#include <rtc.h>

#ifdef NATIVE

#define RISING 1
#define FALLING 2
#define CHANGE 3

#endif

class System {
public:
  System();

  void registerIRQ(int pinnum, void (*fn)(), int mode);
  void feedWatchdog();

  Graphics *getGraphics();
  RTC *getRTC();
  Battery *getBattery();

private:
  Graphics *_gfx;
  RTC *_rtc;
  Battery *_battery;
};

#endif
