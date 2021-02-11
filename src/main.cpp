#include <Arduino.h>
#include "pins.h"

#include "battery/battery.h"
#include "backlight/backlight.h"
#include "graphics/graphics.h"
#include "rtc/rtc.h"
#include "system/system.h"
#include "util/util.h"

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

void setup() {
  sys = new System();

  Graphics *gfx = sys->getGraphics();
  gfx->clearDisplay();

  sys->registerIRQ(PIN_SQW, doit, RISING);
  sys->registerIRQ(PIN_BUTTON1, enter_dfu_if_btns15, CHANGE);
}

void doit() {

  Graphics *gfx = sys->getGraphics();
  RTC *rtc = sys->getRTC();
  Battery *battery = sys->getBattery();

  gfx->clearBuffer();

  RTCDateTime now = rtc->now();

  gfx->setFont(&Dustfine72pt7b);
  gfx->setCursor(55, 120);
  char txt[64] = {'\0'};
  gfx->setTextColor(0);
  gfx->printf("%02d%c", now.hour, ((now.second % 2) ? '.' : ' '));
  gfx->print(txt);

  gfx->setCursor(55, 220);
  gfx->printf("%02d", now.minute);
  gfx->print(txt);

  gfx->setFont(&FreeMonoBold12pt7b);
  gfx->setCursor(30, 250);
  gfx->printf("%04d-%02d-%02d", now.year, now.month, now.day);
  gfx->print(txt);

  gfx->setCursor(25, 270);
  gfx->print("batt: ");
  gfx->print(battery->get_percent());
  gfx->print("%/");
  gfx->printf("%.1fV", (float)(battery->get_voltage_mV())/1000);

  gfx->setCursor(25, 290);
  //gfx->printf("%d uA", (battery->get_current_uA()));

  gfx->refresh();

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
