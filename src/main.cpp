
#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

#include <fonts/Dustfine72pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "stdio.h"

Graphics gfx = Graphics();

extern void doit();

void setup() {
  init_peripherals();
  rtc_init();

  gfx.clearDisplay();

  //nrfx_spi_uninit();

  attachInterrupt(PIN_SQW, doit, RISING);
}

void doit() {
	RTCDateTime now = rtc_now();

  gfx.clearBuffer();

  gfx.setTextColor(0);

  //char txt[9] = {'\0'};
  //snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%02d%c%02d\n", now.hour, ((now.second % 2) ? ':' : ' '), now.minute);

  gfx.setFont(&Dustfine72pt7b);
  gfx.setCursor(65, 100);
  gfx.print(now.hour);
  if (now.second % 2) {
    gfx.print(".");
  }
  gfx.setCursor(65, 200);
  gfx.print(now.minute);

  gfx.setFont(&FreeMonoBold12pt7b);
  gfx.setCursor(30, 230);
  gfx.print("b: ");
  gfx.print(get_battery_percent());
  gfx.print("%");

  gfx.setCursor(30, 250);

  gfx.print(get_battery_voltage_mv());
  gfx.print(" mV");

  gfx.setCursor(30, 270);

  if (digitalRead(PIN_CHG)) {
    gfx.print("not ");
  } 
  gfx.print("charging");

  //gfx.setCursor(30, 200);

  //gfx.print(get_battery_current_uA());
  //gfx.print(" uA");
  


  gfx.refresh();
}

void loop() {
  // set time with bash command: echo "=$((`date +%s` - (4*3600)))" > /dev/ttyACM0
  if (Serial.available()) {
    switch ((char)Serial.read()) {
      case '=':
        int unixt = Serial.parseInt();
        rtc_set_unixt(unixt);
        break;
    }
  }
  delay(100);
  //suspendLoop();
}
