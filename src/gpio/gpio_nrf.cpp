#ifdef EMBEDDED

#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "gpio.h"
#include "pins.h"

GPIO::GPIO(int pin) {
  handle = NULL;
  _pin = pin;
}

void GPIO::enableFor(unsigned ms) {
  if (handle != NULL) {
    vTaskDelete(handle);
    handle = NULL;
  }
  xTaskCreate(
      [](void* x) { GPIO::backlightControlCallback(x); },
      "backlight",
      256, /* stack size, words */
      (void*)(ms),
      1,
      &(this->handle)
  );
}

void GPIO::backlightControlCallback(void* duration_ms) {
  unsigned ms = (unsigned)duration_ms;
  digitalWrite(PIN_BACKLIGHT, HIGH);
  delay(ms);
  digitalWrite(PIN_BACKLIGHT, LOW);
  // suspend the task until it gets cleaned up
  vTaskSuspend(NULL);
}



#endif
