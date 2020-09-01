/* utility functions and one-offs for dev */

#ifndef _UTIL_H_
#define _UTIL_H_

#define DFU_MAGIC_UF2_RESET             0x57

void i2cscan();

/**
 * Enter Device Firmware Update mode.
 * Does not return.
 */
void enter_dfu();

#endif
