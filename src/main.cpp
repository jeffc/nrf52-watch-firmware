
#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

#include <fonts/Dustfine72pt7b.h>
#include <fonts/FreeMonoBold12pt7b.h>
#include "stdio.h"

Graphics gfx = Graphics();

extern void doit();

void setup() {
  init_peripherals();
  battery_setup();
  rtc_init();

  gfx.clearDisplay();

  //nrfx_spi_uninit();

#ifdef EMBEDDED
  attachInterrupt(PIN_SQW, doit, RISING);
#endif
}

void doit() {
	RTCDateTime now = rtc_now();

  gfx.clearBuffer();

  gfx.setTextColor(0);


  gfx.setFont(&Dustfine72pt7b);
  gfx.setCursor(65, 120);
  char txt[64] = {'\0'};
  snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%02d%c", now.hour, ((now.second % 2) ? '.' : ' '));
  gfx.print(txt);
  //if (now.second % 2) {
  //  gfx.print(".");
  //}
  gfx.setCursor(65, 220);
  snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%02d", now.minute);
  gfx.print(txt);

  gfx.setFont(&FreeMonoBold12pt7b);
  gfx.setCursor(30, 250);
  snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%04d-%02d-%02d", now.year, now.month, now.day);
  gfx.print(txt);

  gfx.setCursor(30, 270);
  gfx.print("batt: ");
  gfx.print(get_battery_percent());
  gfx.print("%");

  gfx.setCursor(30, 290);

  if (get_battery_current_uA() < 0) {
    int tte = get_battery_TTE();
    int totalmins = tte / 60;
    if (totalmins > 5760) {
      snprintf(txt, sizeof(txt)/sizeof(txt[0]), "> 4 days left");
    } else {
      if (totalmins >= (24*60)) {
        snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%d:%02d:%02d left", totalmins / (24*60), (totalmins % (24*60))/ 60, totalmins % 60);
      } else {
        snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%d:%02d left", totalmins / 60, totalmins % 60);
      }
    }
  } else {
    int ttf = get_battery_TTF();
    int totalmins = ttf / 60;
    if (totalmins > 0) {
      snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%d:%02d to full", totalmins / 60, totalmins % 60);
    } else {
      snprintf(txt, sizeof(txt)/sizeof(txt[0]), "fully charged");
    }
  }
  gfx.print(txt);

  //gfx.setCursor(30, 200);

  //gfx.print(get_battery_current_uA());
  //gfx.print(" uA");
  


  gfx.refresh();

#ifdef EMBEDDED
  // feed the watchdog
  NRF_WDT->RR[0] = WDT_RR_RR_Reload;
#endif
}

void loop() {
#ifdef EMBEDDED
  // set time with bash command: echo "=$((`date +%s` - (4*3600)))" > /dev/ttyACM0
  if (Serial.available()) {
    switch ((char)Serial.read()) {
      case '=': {
        int unixt = Serial.parseInt();
        rtc_set_unixt(unixt);
        Serial.println("set time");
        break;
      }
      case 'b': {
        battery_model_set();
        Serial.println("set battery model");
        break;
      }
    }
  }
  delay(100);
  //suspendLoop();
 #endif
}
