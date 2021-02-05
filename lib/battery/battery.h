#ifndef _BATTERY_H_
#define _BATTERY_H_

#define BATTERY_I2C_ADDR 0x36

#define REG_STATUS     0x00
#define REG_FSTAT      0x3D
#define REG_HIBCFG     0xBA
#define REG_SOFTWAKE   0x60
#define REG_DESIGNCAP  0x18
#define REG_MODELCFG   0xDB
#define REG_ICHGTERM   0x1E
#define REG_VEMPTY     0x3A

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
