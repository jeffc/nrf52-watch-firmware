#ifdef EMBEDDED

#include <Arduino.h>
#include "FreeRTOS.h"
#include "task.h"
#include "backlight.h"
#include "pins.h"

Backlight::Backlight() {
  handle = NULL;
}

void Backlight::enableFor(unsigned ms) {
  if (handle != NULL) {
    vTaskDelete(handle);
    handle = NULL;
  }
  xTaskCreate(
      Backlight::backlightControlCallback,
      "backlight",
      256, /* stack size, words */
      (void*)(ms),
      1,
      &(this->handle)
  );
}

void Backlight::backlightControlCallback(void* duration_ms) {
  unsigned ms = (unsigned)duration_ms;
  digitalWrite(PIN_BACKLIGHT, HIGH);
  delay(ms);
  digitalWrite(PIN_BACKLIGHT, LOW);
  // suspend the task until it gets cleaned up
  vTaskSuspend(NULL);
}



#endif
