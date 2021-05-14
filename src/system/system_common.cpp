#include <system/system.h>
#include <vector>
#include <views/views.h>
#include <views/WatchFace.h>
#include "pins.h"
#include <iostream>
#include <algorithm>


System* System::_INSTANCE = NULL;

void System::commonSetup() {
  System::_INSTANCE = this;

  _gfx = new Graphics();
  _rtc = new RTC();
  _battery = new Battery();
  _accel = new Accelerometer();
  _backlight = new GPIO(PIN_BACKLIGHT);
  _flashlight = new GPIO(PIN_FLASHLIGHT);
  _5Vreg = new GPIO(PIN_5VREG);

  _event_handlers = std::vector<void(*)(EVENT_T)>();
  initViews();
  setupButtonEvents();
}

void System::setupButtonEvents() {
  registerIRQ(PIN_BUTTON1, fireButtonEvents<PIN_BUTTON1, BUTTON_TOP>, CHANGE);
  registerIRQ(PIN_BUTTON3, fireButtonEvents<PIN_BUTTON3, BUTTON_BOTTOM>, CHANGE);
  registerIRQ(PIN_BUTTON5, fireButtonEvents<PIN_BUTTON5, BUTTON_MIDDLE>, CHANGE);
}

// force-clears the view stack
void System::initViews() {
  if (!_view_stack) {
    _view_stack = new std::list<View*>();
  }
  while (_view_stack->size() > 0) {
    View* v = _view_stack->back();
    _view_stack->pop_back();
    delete v;
  }
  _view_stack->push_back(new WatchFace(this));
}

View* System::getActiveView() {
  std::list<View*> views_to_clean;

  while (_view_stack->size() > 1 &&_view_stack->back()->isDead()) {
    View* v = _view_stack->back();
    _view_stack->pop_back();
    views_to_clean.push_back(v);
  }

  // clean up in two passes to avoid an interrupt using after free
  // this is technically still not thread safe, except that I *think* interrupts
  // on the nrf52840 can't preempt each other
  while(views_to_clean.size() > 0) {
    View* v = views_to_clean.back();
    views_to_clean.pop_back();
    delete v;
  }
  return _view_stack->back();
}

void System::switchToNewView(View* v) {
  _view_stack->push_back(v);
  refreshDisplay();
}

void System::fireEvent(EVENT_T e) {
  for (auto f : _event_handlers) {
    f(e);
  }
  getActiveView()->handleEvent(e);
}

void System::registerEventHandler(void (*handler)(EVENT_T)) {
  _event_handlers.push_back(handler);
}

// this should be mutex-protected, but std::mutex doesn't work on nrf52
// the worst case is one second of corrupted display data, so I'm not terribly
// concerned by the lack of thread safety.
void System::refreshDisplay() {
  getGraphics()->clearBuffer();
  getActiveView()->draw();
  getGraphics()->refresh();
}

Graphics *System::getGraphics() { return _gfx; }
RTC *System::getRTC() { return _rtc; }
Battery *System::getBattery() { return _battery; }
GPIO *System::getBacklight() { return _backlight; }
GPIO *System::getFlashlight() { return _flashlight; }
GPIO *System::get5Vreg() { return _5Vreg; }

