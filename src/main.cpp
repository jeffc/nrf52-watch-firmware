#include <Arduino.h>
#include "pins.h"

#include "battery/battery.h"
#include "gpio/gpio.h"
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

  //suspendLoop();
}

void doit() {
  sys->refreshDisplay();
  sys->feedWatchdog();
}


void loop() {
  sys->feedWatchdog();
#ifdef EMBEDDED
  // suspendLoop();
#endif
#ifdef EMBEDDED
  RTC *rtc = sys->getRTC();
  Battery *battery = sys->getBattery();
  //GPIO *backlight = sys->getBacklight();
  // set time with bash command: echo "=$((`date +%s` - (4*3600)))" >
  // /dev/ttyACM0
  if (Serial && Serial.available()) {
    switch ((char)Serial.read()) {
      case 'p':
        Serial.println("pong");
        Serial.flush();
        break;
      case '=': {
        int unixt = Serial.parseInt();
        rtc->set_unixt(unixt);
        Serial.println("set time");
        }
        break;
      case 'b': 
        Serial.println("setting battery model");
        battery->set_model();
        Serial.println("set battery model");
        break;
      case 'i': 
        i2cscan();
        break;
      case 'l':
        Serial.println("starting ble");
        sys->getBLE()->run();
        break;
      case ';':
        sys->getBLE()->test();
        break;
      case 'k':
        Serial.println("boop");
        sys->getBLE()->test();
        break;
      case 'u':
        Serial.println("entering DFU");
        Serial.flush();
        enter_dfu();
        break;
      case 'z': 
        Serial.println("toggling buzzer");
        sys->getBuzzer()->toggle();
        break;
      case 'v':
        Serial.println(__DATE__ " " __TIME__);
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
