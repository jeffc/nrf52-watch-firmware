#include <system/system.h>
#include <vector>
#include <views/views.h>
#include <views/WatchFace.h>
#include "pins.h"
#include <iostream>


System* System::_INSTANCE = NULL;

void System::commonSetup() {
  System::_INSTANCE = this;
  _event_handlers = std::vector<void(*)(EVENT_T)>();
  initViews();
  setupButtonEvents();
}

void System::setupButtonEvents() {
  registerIRQ(PIN_BUTTON1, fireButtonEvent<BUTTON_TOP, RISING>, RISING);
  registerIRQ(PIN_BUTTON1, fireButtonEvent<BUTTON_TOP, FALLING>, FALLING);
  registerIRQ(PIN_BUTTON1, fireButtonEvent<BUTTON_TOP, CHANGE>, CHANGE);

  registerIRQ(PIN_BUTTON3, fireButtonEvent<BUTTON_MIDDLE, RISING>, RISING);
  registerIRQ(PIN_BUTTON3, fireButtonEvent<BUTTON_MIDDLE, FALLING>, FALLING);
  registerIRQ(PIN_BUTTON3, fireButtonEvent<BUTTON_MIDDLE, CHANGE>, CHANGE);

  registerIRQ(PIN_BUTTON5, fireButtonEvent<BUTTON_BOTTOM, RISING>, RISING);
  registerIRQ(PIN_BUTTON5, fireButtonEvent<BUTTON_BOTTOM, FALLING>, FALLING);
  registerIRQ(PIN_BUTTON5, fireButtonEvent<BUTTON_BOTTOM, CHANGE>, CHANGE);
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
  while (_view_stack->size() > 1 &&_view_stack->back()->isDead()) {
    View* v = _view_stack->back();
    delete v;
    _view_stack->pop_back();
  }
  return _view_stack->back();
}

void System::switchToNewView(View* v) {
  _view_stack->push_back(v);
}

void System::fireEvent(EVENT_T e) {
#ifdef NATIVE
  std::cout << "fired event" << std::endl;
#endif
  for (auto f : _event_handlers) {
    f(e);
  }
  for (View* v : *_view_stack) {
    v->handleEvent(e);
  }
}

void System::registerEventHandler(void (*handler)(EVENT_T)) {
  _event_handlers.push_back(handler);
}
