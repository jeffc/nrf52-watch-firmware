#include "gpio.h"

bool GPIO::isOn() {
  return _is_on;
}

void GPIO::toggle() {
  if (isOn()) {
    off();
  } else {
    on();
  }
}
