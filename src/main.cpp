
#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

#include <Fonts/FreeMonoBold24pt7b.h>

Graphics gfx = Graphics();

void setup() {
  init_peripherals();
  rtc_init();

  gfx.begin();

  gfx.clearDisplay();
  gfx.setRotation(2);
  gfx.setFont(&FreeMonoBold24pt7b);
  gfx.refresh();

  //nrfx_spi_uninit();

}

void loop() {
	RTCDateTime now = rtc_now();

  gfx.clearBuffer();

  gfx.setCursor(30, 50);
  gfx.setTextColor(0);

  gfx.printf("%02d%c%02d\n", now.hour, ((now.second % 2) ? ':' : ' '), now.minute);


  gfx.refresh();
	delay(500);
}
