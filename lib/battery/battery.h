#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <stdint.h>

void battery_setup();
void battery_model_set();
uint8_t get_battery_percent();
int get_battery_voltage_mv();
int get_battery_current_uA();
int get_battery_TTE();
int get_battery_TTF();

#endif
