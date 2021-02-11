#ifdef EMBEDDED

#include "battery.h"
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

uint16_t i2c_read_register16(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(addr, 2);
  uint16_t out = Wire.read();
  out |= (Wire.read() << 8);

  return out;
}

void i2c_write_register16(uint8_t addr, uint8_t reg, uint16_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write((uint8_t)(val & 0xFF));
  Wire.write((uint8_t)((val >> 8) & 0xFF));
  Wire.endTransmission();
}

void i2c_write_and_verify_register16(uint8_t addr, uint8_t reg, uint16_t val) {
  int attempts = 0;
  do {
    i2c_write_register16(addr, reg, val);
    delay(10);
  } while ( (i2c_read_register16(addr, reg) != val) && attempts++<3);
}

Battery::Battery() {
  return;
  uint16_t status_reg = i2c_read_register16(BATTERY_I2C_ADDR, REG_STATUS);

  Serial.printf("0x%x\r\n", status_reg);

  if (status_reg & 0x02) { // power on reset (POR) bit is set
    // do setup

    int attempts = 0;
    while (i2c_read_register16(BATTERY_I2C_ADDR, REG_FSTAT) & 0x01 && (attempts++ < 1000)) {
      Serial.println("wait1\n");
      Serial.printf("0x%x\r\n", i2c_read_register16(BATTERY_I2C_ADDR, REG_FSTAT));
      delay(10);
    }

    set_model();

    // clear POR bit
    Serial.println("Clearing POR flag");
    i2c_write_and_verify_register16(BATTERY_I2C_ADDR, REG_STATUS, status_reg & 0xFFFD);
  }
}

void Battery::set_model() {
  // get hibcfg value
  uint16_t hibcfg = i2c_read_register16(BATTERY_I2C_ADDR, REG_HIBCFG);
  // wake up (3 steps, per manufacturer reference)
  i2c_write_register16(BATTERY_I2C_ADDR, REG_SOFTWAKE, 0x0090);
  i2c_write_register16(BATTERY_I2C_ADDR, REG_HIBCFG,   0x0000);
  i2c_write_register16(BATTERY_I2C_ADDR, REG_SOFTWAKE, 0x0000);

  // designcap
  i2c_write_register16(BATTERY_I2C_ADDR, REG_DESIGNCAP, 1000); // 0.5mAh resolution * 500mAh = 1000

  // IchgTerm ; according to datasheet, this is 7.5% of Ireg, which is
  // 1000V/Rreg, which is 500mA, so IchgTerm = 37.5mA. At 156.25 uA
  // resolution, this gives a register value of 240
  i2c_write_register16(BATTERY_I2C_ADDR, REG_ICHGTERM, 240);

  // Vempty. 2.8V / (1.25mV/16) resolution = 35840
  i2c_write_register16(BATTERY_I2C_ADDR, REG_VEMPTY, 0xA561);

  // write model
  i2c_write_register16(BATTERY_I2C_ADDR, REG_MODELCFG, 0x8000);
  // wait for refresh bit to clear
  int attempts = 0;
  while (i2c_read_register16(BATTERY_I2C_ADDR, REG_MODELCFG) & 0x8000 && (attempts++<1000)) {
    Serial.println("wait2");
    delay(10); 
  }

  i2c_write_register16(BATTERY_I2C_ADDR, REG_HIBCFG, hibcfg);
}

uint8_t Battery::get_percent() {
  uint16_t battery_pct = i2c_read_register16(BATTERY_I2C_ADDR, 0x06);
  uint8_t bpct_h = (battery_pct >> 8) & 0xFF;
  return bpct_h;
}

int Battery::get_voltage_mV() {
  uint16_t reg = i2c_read_register16(BATTERY_I2C_ADDR, 0x09);
  return (reg * 1.25) / 16;
}

int Battery::get_current_uA() {
  int16_t reg = i2c_read_register16(BATTERY_I2C_ADDR, 0x0B);
  return (reg * 156.25);
}

int Battery::get_TTE() {
  uint64_t reg = i2c_read_register16(BATTERY_I2C_ADDR, 0x11);
  return (reg * 5.625);
}

int Battery::get_TTF() {
  int16_t reg = i2c_read_register16(BATTERY_I2C_ADDR, 0x20);
  return (reg * 5.625);
}

#endif
