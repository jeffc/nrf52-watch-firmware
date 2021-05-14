#ifdef EMBEDDED

#include <string>
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
  _on_for = ms;
  // use a lambda that takes `this` as a parameter, as a "poor man's capture"
  xTaskCreate(
      [](void* x) { GPIO* me = (GPIO*)x; me->on(); delay(me->_on_for); me->off(); vTaskSuspend(NULL); },
      ("gpio_" + std::to_string(_pin)).c_str(),
      256, /* stack size, words */
      (void*)this,
      1,
      &(this->handle)
  );
}

void GPIO::on() {
  digitalWrite(_pin, HIGH);
}

void GPIO::off() {
  digitalWrite(_pin, LOW);
}

#endif
