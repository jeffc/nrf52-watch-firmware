#ifdef NATIVE

#include <string>
#include <iostream>
#include <Arduino.h>
#include "gpio.h"

GPIO::GPIO(int pin) {
  _pin = pin;
}

void GPIO::enableFor(unsigned ms) {
  std::cout << "pretending to enable pin " << _pin << " for " << ms << "ms" << std::endl;
}

void GPIO::on() {
  std::cout << "pretending to enable pin " << _pin << std::endl;
}

void GPIO::off() {
  std::cout << "pretending to disable pin " << _pin << std::endl;
}

#endif
