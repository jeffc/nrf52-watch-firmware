#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifdef NATIVE

#define RISING  1
#define FALLING 2
#define CHANGE  3

#endif


void init_peripherals();

void registerIRQ(int pinnum, void (*fn)(), int mode);

void feed_watchdog();

#endif
