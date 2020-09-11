#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <stdint.h>

class Battery {
public:
  Battery();

  void set_model();
  uint8_t get_percent();
  int get_voltage_mV();
  int get_current_uA();
  int get_TTE();
  int get_TTF();
};

#endif
