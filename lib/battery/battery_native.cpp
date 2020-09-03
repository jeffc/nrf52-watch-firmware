#ifdef NATIVE

#include "stdio.h"
#include "battery.h"

void battery_setup() {
  printf("Called battery_setup()\n");
}

uint8_t get_battery_percent() { return 100; }
int get_battery_voltage_mv() { return 4000; }
int get_battery_current_uA() { return 0; }
int get_battery_TTE() { return 60*60*24*4 + 1; }
int get_battery_TTF() { return 0; }

#endif
