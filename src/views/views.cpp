#include "system/system.h"
#include "views/views.h"

View::View(System* s) {
  _sys = s;
}

void View::draw() {
}

void View::update() {
}

View::~View() {
}

void View::handleEvent(EVENT_T e) {
}

bool View::isDead() {
  return _is_dead;
}

void View::exit() {
  _is_dead = true;
  _sys->refreshDisplay();
}
