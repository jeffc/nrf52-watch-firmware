#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>
#include "stdio.h"

System* sys;

extern void doit();

void enter_dfu_if_btns15() {
#ifdef EMBEDDED
  Graphics* gfx = sys->getGraphics();
  if (!digitalRead(PIN_BUTTON1) && !digitalRead(PIN_BUTTON5)) {
    gfx->clearBuffer();
    gfx->setCursor(20, 100);
    gfx->printf("Device\nUpdate\nMode");
    gfx->refresh();
    enter_dfu();
  }
#endif
}


void setup() {
  sys = new System();

  Graphics* gfx = sys->getGraphics();
  gfx->clearDisplay();

  sys->registerIRQ(PIN_SQW, doit, RISING);
  sys->registerIRQ(PIN_BUTTON1, enter_dfu_if_btns15, CHANGE);
}

void doit() {

  Graphics* gfx = sys->getGraphics();
  RTC* rtc = sys->getRTC();
  Battery* battery = sys->getBattery();

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

  gfx->setCursor(30, 270);
  gfx->print("batt: ");
  gfx->print(battery->get_percent());
  gfx->print("%");

  gfx->setCursor(30, 290);

  if (battery->get_current_uA() < 0) {
    int tte = battery->get_TTE();
    int totalmins = tte / 60;
    if (totalmins > 5760) {
      gfx->printf("> 4 days left");
    } else {
      if (totalmins >= (24*60)) {
        gfx->printf("%d:%02d:%02d left", totalmins / (24*60), (totalmins % (24*60))/ 60, totalmins % 60);
      } else {
        gfx->printf("%d:%02d left", totalmins / 60, totalmins % 60);
      }
    }
  } else {
    int ttf = battery->get_TTF();
    int totalmins = ttf / 60;
    if (totalmins > 0) {
      gfx->printf("%d:%02d to full", totalmins / 60, totalmins % 60);
    } else {
      gfx->printf("fully charged");
    }
  }
  gfx->print(txt);

  gfx->refresh();

  sys->feedWatchdog();
}

void loop() {
#ifdef EMBEDDED
  //suspendLoop();
#endif
#ifdef EMBEDDED
  RTC* rtc = sys->getRTC();
  Battery* battery = sys->getBattery();
  // set time with bash command: echo "=$((`date +%s` - (4*3600)))" > /dev/ttyACM0
  if (Serial.available()) {
    switch ((char)Serial.read()) {
      case '=': {
        int unixt = Serial.parseInt();
        rtc->set_unixt(unixt);
        Serial.println("set time");
        break;
      }
      case 'b': {
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
      }
    }
  }
  delay(1000);
 #endif
}
