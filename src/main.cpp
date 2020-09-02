
#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
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

  gfx.setCursor(30, 50);
  gfx.setTextColor(0);

  char txt[9] = {'\0'};
  snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%02d%c%02d\n", now.hour, ((now.second % 2) ? ':' : ' '), now.minute);

  gfx.setFont(&FreeMonoBold24pt7b);
  gfx.print(txt);

  gfx.setFont(&FreeMonoBold18pt7b);
  gfx.setCursor(30, 100);
  gfx.print("b: ");
  gfx.print(get_battery_percent());
  gfx.print("%");

  gfx.setCursor(30, 150);

  gfx.print(get_battery_voltage_mv());
  gfx.print(" mV");

  gfx.setCursor(30, 200);

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
