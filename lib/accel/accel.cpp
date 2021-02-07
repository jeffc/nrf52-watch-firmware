#include "accel.h"
#include <Wire.h>
#include <Arduino.h>

Accelerometer::Accelerometer() {
  // soft reset
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_CMD);
  Wire.write(BMA400_CMD_RESET);
  Wire.endTransmission();

  delay(10); // delay after soft reset

  // Set normal mode
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_ACC_CONFIG0);
  Wire.write(BMA400_MODE_NORMAL);
  Wire.endTransmission();

  // Set osr to 3 (max sample rate)
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_ACC_CONFIG1);
  Wire.write(BMA400_OSR_HIGH | BMA400_ODR_200HZ);
  Wire.endTransmission();

  // Enable double tap detection interrupt
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_INT_CONFIG1);
  Wire.write(BMA400_DOUBLE_TAP_INT);
  Wire.endTransmission();

  // Set tap sensitivity to 4 (~halfway);
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_TAP_CONFIG);
  Wire.write(7);
  Wire.endTransmission();


  // Map tap interrupt to INT1
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_INT12_MAP);
  Wire.write(BMA400_TAP_INT1);
  Wire.endTransmission();

  // Set interrupt 1 active high
  Wire.beginTransmission(BMA400_I2C_ADDR);
  Wire.write(BMA400_REG_INT12_IO_CTRL);
  Wire.write(BMA400_INT1_ACTIVE_HIGH);
  Wire.endTransmission();
}
