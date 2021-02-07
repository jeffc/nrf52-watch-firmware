#include "pins.h"

#include "battery.h"
#include "graphics.h"
#include "rtc.h"
#include "system.h"
#include "util.h"

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
    gfx->printf("Device\nUpdate\nMode");
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
  //Battery *battery = sys->getBattery();

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
  //gfx->print("batt: ");
  //gfx->print(battery->get_percent());
  //gfx->print("%");

  //gfx->setCursor(30, 290);

  //if (battery->get_current_uA() < 0) {
  //  int tte = battery->get_TTE();
  //  int totalmins = tte / 60;
  //  if (totalmins > 5760) {
  //    gfx->printf("> 4 days left");
  //  } else {
  //    if (totalmins >= (24 * 60)) {
  //      gfx->printf("%d:%02d:%02d left", totalmins / (24 * 60),
  //                  (totalmins % (24 * 60)) / 60, totalmins % 60);
  //    } else {
  //      gfx->printf("%d:%02d left", totalmins / 60, totalmins % 60);
  //    }
  //  }
  //} else {
  //  int ttf = battery->get_TTF();
  //  int totalmins = ttf / 60;
  //  if (totalmins > 0) {
  //    gfx->printf("%d:%02d to full", totalmins / 60, totalmins % 60);
  //  } else {
  //    gfx->printf("fully charged");
  //  }
  //}
  gfx->print(txt);

  gfx->refresh();

  sys->feedWatchdog();
}

#ifdef EMBEDDED
#include <Wire.h>

void intcalled() {
  Serial.println("Interrupt!");
  Serial.print("Reading interrupt status register: ");
  Wire.beginTransmission(0x14);
  Wire.write(0x0F); // INT_STAT0
  Wire.endTransmission();
  Wire.requestFrom(0x14, 1);
  Serial.printf("0x%02x\r\n", Wire.read());
}
#endif

void loop() {
#ifdef EMBEDDED
  // suspendLoop();
#endif
#ifdef EMBEDDED
  RTC *rtc = sys->getRTC();
  Battery *battery = sys->getBattery();
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
      case 'a': {
        sys->registerIRQ(PIN_ACCELINT1, intcalled, RISING);
        Serial.println("initializing accelerometer");
        Serial.flush();
        Wire.beginTransmission(0x14);
        Wire.write(0x7E); // CMD
        Wire.write(0xB6); // soft reset
        Wire.endTransmission();

        delay(10);

        Wire.beginTransmission(0x14);
        Wire.write(0x00);
        Wire.endTransmission();
        Wire.requestFrom(0x14, 1);
        uint8_t  chipid = Wire.read();
        Serial.print("got chip id ");
        Serial.println(chipid, HEX);

        // Set normal mode
        Wire.beginTransmission(0x14);
        Wire.write(0x19); // ACC_CONFIG0
        Wire.write(0x02); // normal mode
        Wire.endTransmission();

        // Set osr to 3 (max sample rate)
        Wire.beginTransmission(0x14);
        Wire.write(0x1A); // ACC_CONFIG1
        Wire.write(0x79); // osr high, 200hz
        Wire.endTransmission();

        // Enable tap detection interrupts
        Wire.beginTransmission(0x14);
        Wire.write(0x20); // INT_CONFIG1
        Wire.write(0x0C); // d_tap_int_en | s_tap_int_en
        Wire.endTransmission();

        // Map tap interrupt to INT1
        Wire.beginTransmission(0x14);
        Wire.write(0x23); // INT12_MAP
        Wire.write(0x04); // tap_int1
        Wire.endTransmission();

        // Set interrupts active high
        Wire.beginTransmission(0x14);
        Wire.write(0x24); // INT12_IO_CTRL
        Wire.write(0x22);
        Wire.endTransmission();
        break;
      }
      case 'x': {
        pinMode(PIN_ACCELINT1, INPUT);
        pinMode(PIN_ACCELINT2, INPUT);
        Serial.print("Interrupt pins: ");
        Serial.print(digitalRead(PIN_ACCELINT1));
        Serial.print(" ");
        Serial.print(digitalRead(PIN_ACCELINT2));
        Serial.println("");

        Serial.print("Reading interrupt status registers: ");
        Wire.beginTransmission(0x14);
        Wire.write(0x0E); // INT_STAT0
        Wire.endTransmission();
        Wire.requestFrom(0x14, 3);
        Serial.printf("0x%02x, 0x%02x 0x%02x\r\n", Wire.read(), Wire.read(), Wire.read());

        Serial.print("Config registers: ");
        Wire.beginTransmission(0x14);
        Wire.write(0x1F); // INT_CONFIG0
        Wire.endTransmission();
        Wire.requestFrom(0x14, 6);
        for(int i =0; i < 6; i++) {
          Serial.printf("0x%02x ", Wire.read());
        }
        Serial.println();
        break;
      }
    }
  }
  delay(100);
#endif
}
