#ifdef NATIVE

#include "stdio.h"
#include "battery.h"

Battery::Battery() {
  printf("Called battery_setup()\n");
}

uint8_t Battery::get_percent() { return 100; }
int Battery::get_voltage_mV() { return 4000; }
int Battery::get_current_uA() { return 0; }
int Battery::get_TTE() { return 60*60*24*4 + 1; }
int Battery::get_TTF() { return 0; }

#endif
