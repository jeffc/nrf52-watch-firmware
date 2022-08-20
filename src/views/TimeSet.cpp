#include "TimeSet.h"
#include "Menu.h"
#include "util/util.h"

#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>

#include <string>
#include <system/system.h>
#include <utility>
#include <vector>

void TimeSet::draw() {
  Graphics *gfx = _sys->getGraphics();
  RTC *rtc = _sys->getRTC();
  Battery *battery = _sys->getBattery();

  gfx->clearBuffer();

  RTCDateTime now = rtc->now();

  gfx->setTextColor(0);
  gfx->setFont(&Dustfine72pt7b);
  gfx->setCursor(45, 120);
  //char txt[64] = {'\0'};
  gfx->setTextColor(0);
  //gfx->printf("%02d%c", now.hour, ((now.second % 2) ? '.' : ' '));
  gfx->printf("%02d", now.hour);
  //gfx->print(txt);

  gfx->setFont(&Dustfine72pt7b);
  gfx->setCursor(45, 220);
  gfx->printf("%02d", now.minute);

  gfx->setFont(&FreeMonoBold12pt7b);
  gfx->printf("%02d", now.second);

  gfx->setFont(&FreeMonoBold12pt7b);
  gfx->setCursor(15, 250);
  const char* dow = "???";
  switch (now.dayOfWeek) {
    case 0:
      dow = "SUN";
      break;
    case 1:
      dow = "MON";
      break;
    case 2:
      dow = "TUE";
      break;
    case 3:
      dow = "WED";
      break;
    case 4:
      dow = "THU";
      break;
    case 5:
      dow = "FRI";
      break;
    case 6:
      dow = "SAT";
      break;
  }

  gfx->printf("%s %04d-%02d-%02d", dow, now.year, now.month, now.day);

  gfx->setCursor(15, 270);
  gfx->print("batt: ");
  gfx->print(battery->get_percent());
  gfx->print("%/");
  gfx->printf("%.1fV", (float)(battery->get_voltage_mV())/1000);

  gfx->setCursor(25, 290);
  //gfx->printf("%d uA", (battery->get_current_uA()));
  //
  //drawTopBar(_sys);
}

void TimeSet::handleEvent(EVENT_T e) {
  // flashlight
  if (e.type == BUTTON_CHANGE && e.button == BUTTON_MIDDLE) {
    if (_sys->getButtonPressed(PIN_BUTTON5)) {
      _sys->getFlashlight()->on();
    } else {
      _sys->getFlashlight()->off();
    }
  }

  if (e.type == BUTTON_PRESS && e.button == BUTTON_BOTTOM) {
    Menu* menu = new Menu(_sys);
    menu->addItem([&]() { return std::string((_sys->getBacklight()->isOn()) ? "disable" : "enable") + " backlight";}, [&]() { _sys->getBacklight()->toggle(); return false; });
    menu->addItem([&]() { return std::string((_sys->get5Vreg()->isOn()) ? "disable" : "enable") + " 5v reg";}, [&]() { _sys->get5Vreg()->toggle(); return false; });
    menu->addItem([&]() { return std::string((_sys->getFlashlight()->isOn()) ? "disable" : "enable") + " flashlight";}, [&]() { _sys->getFlashlight()->toggle(); return false; });
    menu->addItem([&]() { return std::string((_sys->isSerialEnabled()) ? "disable" : "enable") + " serial";}, [&]() { 
        if (_sys->isSerialEnabled()) {
          _sys->disableSerial();
        } else {
          _sys->enableSerial();
        }
        return false;});

    menu->addItem("", []() { return false; }); // spacer
    menu->addItem("Update Mode", [&]() { _sys->getFlashlight()->off(); enter_dfu(); return false; });
    menu->addItem("back", []() { return true; });
    _sys->switchToNewView(menu);
  }
}
