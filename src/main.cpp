#include <Arduino.h>
#include "pins.h"

#include "battery/battery.h"
#include "backlight/backlight.h"
#include "graphics/graphics.h"
#include "rtc/rtc.h"
#include "system/system.h"
#include "util/util.h"

#include "views/views.h"
#include "views/PopupView.h"

#include "stdio.h"
#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>

System *sys;

extern void doit();

void enter_dfu_if_btns15(EVENT_T e) {
  Graphics *gfx = sys->getGraphics();
  if (sys->getButtonPressed(PIN_BUTTON1) && sys->getButtonPressed(PIN_BUTTON5)) {
    gfx->clearBuffer();
    gfx->setCursor(20, 100);
    gfx->printf("Device\n  Update\n  Mode");
    gfx->refresh();
    enter_dfu();
  }
}

void setup() {
  sys = new System();

  Graphics *gfx = sys->getGraphics();
  gfx->clearDisplay();

  sys->registerIRQ(PIN_SQW, doit, RISING);
  sys->registerEventHandler(enter_dfu_if_btns15);
}

void doit() {
  sys->refreshDisplay();
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
      case 'v':
        Serial.println(__DATE__ " " __TIME__);
        break;
      case 'x':
        sys->fireEvent({BUTTON_PRESS, BUTTON_BOTTOM});
        break;

      case '1':
        Serial.println("pressing TOP");
        sys->fireEvent({BUTTON_PRESS, BUTTON_TOP});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_TOP});
        break;

      case '8':
        Serial.println("releasing TOP");
        sys->fireEvent({BUTTON_RELEASE, BUTTON_TOP});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_TOP});
        break;
      case '2':
        Serial.println("pressing MIDDLE");
        sys->fireEvent({BUTTON_PRESS, BUTTON_MIDDLE});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_MIDDLE});
        break;

      case '9':
        Serial.println("releasing MIDDLE");
        sys->fireEvent({BUTTON_RELEASE, BUTTON_MIDDLE});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_MIDDLE});
        break;

      case '3':
        Serial.println("pressing BOTTOM");
        sys->fireEvent({BUTTON_PRESS, BUTTON_BOTTOM});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_BOTTOM});
        break;

      case '0':
        Serial.println("releasing BOTTOM");
        sys->fireEvent({BUTTON_RELEASE, BUTTON_BOTTOM});
        sys->fireEvent({BUTTON_CHANGE, BUTTON_BOTTOM});
        break;

      case 't':
        Serial.println("Manually calling doit()");
        doit();
        break;
    }
  }
  delay(100);
#endif
}
