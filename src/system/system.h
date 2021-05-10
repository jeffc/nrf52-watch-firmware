#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <battery/battery.h>
#include <backlight/backlight.h>
#include <graphics/graphics.h>
#include <accel/accel.h>
#include <rtc/rtc.h>
#include <mutex>
#include <set>
#include <stack>

#include <views/views.h>

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
  Backlight *getBacklight();

  View* getActiveView();
  void switchToNewView(View* v);

  bool getButtonPressed(int pin);

#ifdef NATIVE
  // this happens in hardware on the nrf52
  void fireIRQ(int pin, int mode);

  // there are a lot of awful things we have to do to get the native SDL graphics
  // rendering threads to play nice with our interrupt-driven single-core-CPU
  // code. The mutex effectively emulates that core, and we lock/unlock the
  // display (or the thread updating it).
  static std::mutex runlock;

  // also keep track of which keys are down (from SDL)
  static std::set<int> buttonsPressed;
#endif

private:

  void initBaseView();

  Graphics *_gfx;
  RTC *_rtc;
  Battery *_battery;
  Accelerometer *_accel;
  Backlight *_backlight;
  View *_active_view;

  std::stack<View*> _view_stack;
};

#endif
