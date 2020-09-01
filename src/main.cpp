
#include "pins.h"

#include "battery.h"
#include "util.h"
#include "rtc.h"
#include "graphics.h"
#include "system.h"

//#include <Fonts/FreeMonoBold24pt7b.h>
#include "stdio.h"

Graphics gfx = Graphics();

void setup() {
  init_peripherals();
  rtc_init();

  gfx.clearDisplay();
  //gfx.setFont(&FreeMonoBold24pt7b);

  //nrfx_spi_uninit();

}

void loop() {
	RTCDateTime now = rtc_now();

  gfx.clearBuffer();

  gfx.setCursor(30, 50);
  gfx.setTextColor(0);

  char txt[8];
  snprintf(txt, sizeof(txt)/sizeof(txt[0]), "%02d%c%02d\n", now.hour, ((now.second % 2) ? ':' : ' '), now.minute);

  gfx.print(txt);


  gfx.refresh();
#ifdef EMBEDDED
	delay(500);
#endif
}
