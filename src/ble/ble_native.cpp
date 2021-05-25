#ifdef NATIVE

#include <iostream>
#include "ble.h"

BLE::BLE() {
}

void BLE::enable() {
  _is_enabled = true;
  std::cout << "Enabling BLE" << std::endl;
}

void BLE::disable() {
  _is_enabled = false;
  std::cout << "Disabling BLE" << std::endl;
}

#endif
