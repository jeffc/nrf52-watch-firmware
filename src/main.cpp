#include <Arduino.h>
#include "pins.h"

#include "battery/battery.h"
#include "backlight/backlight.h"
#include "graphics/graphics.h"
#include "rtc/rtc.h"
#include "system/system.h"
#include "util/util.h"

#include "views/PopupView.h"

#include "stdio.h"
#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>

System *sys;

extern void doit();

void enter_dfu_if_btns15() {
  Graphics *gfx = sys->getGraphics();
  if (sys->getButtonPressed(PIN_BUTTON1) && sys->getButtonPressed(PIN_BUTTON5)) {
    gfx->clearBuffer();
    gfx->setCursor(20, 100);
    gfx->printf("Device\n  Update\n  Mode");
    gfx->refresh();
    enter_dfu();
  }
}

void switchView() {
  sys->switchToNewView(new PopupView(sys));
}

void setup() {
  sys = new System();

  Graphics *gfx = sys->getGraphics();
  gfx->clearDisplay();

  sys->registerIRQ(PIN_SQW, doit, RISING);
  sys->registerIRQ(PIN_BUTTON1, enter_dfu_if_btns15, CHANGE);
  sys->registerIRQ(PIN_BUTTON5, switchView, RISING);
}

void doit() {

  sys->getGraphics()->clearBuffer();
  sys->getActiveView()->draw();
  sys->getGraphics()->refresh();
  sys->feedWatchdog();
}

void loop() {
#ifdef EMBEDDED
  // suspendLoop();
#endif
#ifdef EMBEDDED
  RTC *rtc = sys->getRTC();
  Battery *battery = sys->getBattery();
  Backlight *backlight = sys->getBacklight();
  // set time with bash command: echo "=$((`date +%s` - (4*3600)))" >
  // /dev/ttyACM0
  if (Serial.available()) {
    switch ((char)Serial.read()) {
      case 'p': {
        Serial.println("pong");
        Serial.flush();
        break;
      }
      case '=': {
        int unixt = Serial.parseInt();
        rtc->set_unixt(unixt);
        Serial.println("set time");
        break;
      }
      case 'b': {
        Serial.println("setting battery model");
        battery->set_model();
        Serial.println("set battery model");
        break;
      }
      case 'i': {
        i2cscan();
        break;
      }
      case 'l': {
        digitalToggle(PIN_FLASHLIGHT);
        Serial.println("toggled light");
        Serial.flush();
        break;
      }
      case 'u': {
        Serial.println("entering DFU");
        Serial.flush();
        enter_dfu();
        break;
      }
    }
  }
  delay(100);
#endif
}
