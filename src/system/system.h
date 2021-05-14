#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <battery/battery.h>
#include <gpio/gpio.h>
#include <graphics/graphics.h>
#include <accel/accel.h>
#include <rtc/rtc.h>
#include <set>
#include <vector>
#include <list>
#include <pins.h>

#include <views/views.h>

#ifdef NATIVE
#include <mutex>
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
  GPIO *getBacklight();
  GPIO *getFlashlight();
  GPIO *get5Vreg();
  GPIO *getBuzzer();

  View* getActiveView();
  void switchToNewView(View* v);

  bool getButtonPressed(int pin);
  void fireEvent(EVENT_T e);

  void registerEventHandler(void (*handler)(EVENT_T));

  static System* getInstance() {return _INSTANCE;};

  void refreshDisplay();

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
  GPIO *_backlight;
  GPIO *_flashlight;
  GPIO *_5Vreg;
  GPIO *_buzzer;

  static System* _INSTANCE;

  std::list<View*>* _view_stack;
  std::vector<void (*)(EVENT_T)> _event_handlers;
};

/* There's a better way to do this, right? */
template<int pin, BUTTON_T b>
void fireButtonEvents() {
  System* sys = System::getInstance();
  if (sys) {
    if (sys->getButtonPressed(pin)) {
      sys->fireEvent((EVENT_T) { BUTTON_PRESS,  b });
    } else {
      sys->fireEvent((EVENT_T) { BUTTON_RELEASE,  b });
    }
    sys->fireEvent((EVENT_T) { BUTTON_CHANGE,  b });
  }
};


#endif
