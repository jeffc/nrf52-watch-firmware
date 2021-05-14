#include "views/Menu.h"
#include "system/system.h"
#include <fonts/small_pixel8pt7b.h>
#include <iostream>
#include <cassert>

void Menu::draw() {
  Graphics *gfx = _sys->getGraphics();
  gfx->setTextWrap(false);
  gfx->setFont(&small_pixel8pt7b);
  gfx->clearBuffer();

  gfx->setTextColor(0);

  for (int i = 0; i < _items.size(); i++) {
    if (i == _pos) {
      gfx->setCursor(20, 30+20*i);
      gfx->print(">");
    }
    gfx->setCursor(30, 30+20*i);
    gfx->printf("%s\n", _items[i].first().c_str());
  }
}

void Menu::handleEvent(EVENT_T e) {
  if (e.type == BUTTON_PRESS) {
    switch (e.button) {
      case BUTTON_TOP:
        if (_pos > 0) {
          _pos--;
        } else {
          std::cout << "stuck" << std::endl;
        }
        break;
      case BUTTON_BOTTOM:
        if (_pos < _items.size()-1) {
          _pos++;
        } else {
          std::cout << "stuck" << std::endl;
        }
        break;
      case BUTTON_MIDDLE:
        if (_items[_pos].second()) {
          exit();
          return;
        }
        break;
    }
    _sys->refreshDisplay();
  }
}

void Menu::addItem(std::string label, std::function<bool(void)> func) {
  _items.push_back(std::pair<std::function<std::string(void)>, std::function<bool(void)>>([=]() { return label; }, func));
}

void Menu::addItem(std::function<std::string(void)> label_fn, std::function<bool(void)> func) {
  _items.push_back(std::pair<std::function<std::string(void)>, std::function<bool(void)>>(label_fn, func));
}
