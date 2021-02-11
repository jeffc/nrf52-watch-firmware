#ifndef _ACCEL_H_
#define _ACCEL_H_

#define BMA400_I2C_ADDR 0x14

#define BMA400_REG_CMD           0x7E

#define BMA400_REG_ACC_CONFIG0   0x19
#define BMA400_REG_ACC_CONFIG1   0x1A

#define BMA400_REG_INT_CONFIG1   0x20
#define BMA400_REG_INT12_MAP     0x23
#define BMA400_REG_INT12_IO_CTRL 0x24

#define BMA400_REG_TAP_CONFIG    0x57

#define BMA400_CMD_RESET         0xB6

// for ACC_CONFIG0
#define BMA400_MODE_NORMAL       0x02

// for ACC_CONFIG1
#define BMA400_ODR_200HZ         0x09
#define BMA400_OSR_HIGH          0x70

// for INT_CONFIG1
#define BMA400_DOUBLE_TAP_INT    0x08
#define BMA400_SINGLE_TAP_INT    0x04

// for INT12_MAP
#define BMA400_TAP_INT1          0x04

// for INT12_IO_CTRL
#define BMA400_INT1_ACTIVE_HIGH  0x02
#define BMA400_INT2_ACTIVE_HIGH  0x20

class Accelerometer {
  public:
    Accelerometer();
};
    
#endif
