#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <battery/battery.h>
#include <backlight/backlight.h>
#include <graphics/graphics.h>
#include <accel/accel.h>
#include <rtc/rtc.h>
#include <mutex>
#include <set>
#include <vector>
#include <list>
#include <pins.h>

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
  void fireEvent(EVENT_T e);

  void registerEventHandler(void (*handler)(EVENT_T));

  static System* getInstance() {return _INSTANCE;};

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

  void commonSetup();
  void initViews();
  void setupButtonEvents();

  Graphics *_gfx;
  RTC *_rtc;
  Battery *_battery;
  Accelerometer *_accel;
  Backlight *_backlight;
  View *_active_view;

  static System* _INSTANCE;

  std::list<View*>* _view_stack;
  std::vector<void (*)(EVENT_T)> _event_handlers;
};

/* There's a better way to do this, right? */
template<int pin, int mode>
void fireButtonEvent() {
  EVENT_TYPE_T t;
  switch (mode) {
    case FALLING: t = BUTTON_PRESS; break;
    case RISING:  t = BUTTON_RELEASE; break;
    case CHANGE:  t = BUTTON_CHANGE; break;
  }

  BUTTON_T b = BUTTON_TOP; // to prevent unassigned
  switch (pin) {
    case PIN_BUTTON1: b = BUTTON_TOP; break;
    case PIN_BUTTON3: b = BUTTON_MIDDLE; break;
    case PIN_BUTTON5: b = BUTTON_BOTTOM; break;
  }

  System* sys = System::getInstance();
  if (sys) {
    sys->fireEvent((EVENT_T) { t,  b });
  }
};


#endif
