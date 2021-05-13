#include <system/system.h>
#include "WatchFace.h"
#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>
/*
void drawTopBar(System* _sys) {
  Graphics *gfx = _sys->getGraphics();
  Battery *battery = _sys->getBattery();

  gfx->fillRect(0, 0, SCREEN_WIDTH, 10, 0);
  gfx->setTextColor(1);
  gfx->setCursor(4,8);
  gfx->setFont(&Org_01);
  gfx->printf("%d mV", (battery->get_voltage_mV()));

  int16_t x1, y1;
  uint16_t w, h;
  char buf[32] = {'\0'};
  snprintf(buf, sizeof(buf)-1, "%d%%", battery->get_percent());
  gfx->getTextBounds(buf, 0, 0, &x1, &y1, &w, &h);
  gfx->setCursor(SCREEN_WIDTH - w - 4, 8);
  gfx->print(buf);
}
*/


void WatchFace::draw() {
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
  gfx->setCursor(30, 250);
  gfx->printf("%04d-%02d-%02d", now.year, now.month, now.day);

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
