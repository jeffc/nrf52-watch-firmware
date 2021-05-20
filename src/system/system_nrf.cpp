#ifdef EMBEDDED

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "accel/accel.h"
#include "gpio/gpio.h"
#include "battery/battery.h"
#include "pins.h"
#include "system/system.h"
#include "util/util.h"

// global hack for interrupts

static System *sys = NULL;

// Fault handlers
// https://www.freertos.org/Debugging-Hard-Faults-On-Cortex-M-Microcontrollers.html

void HardFault_Handler() {
  enter_dfu();
}

// docs conflict on what the name for this function should be. FreeRTOS (link
// above) says MemMang_, but everywhere else says MemoryManagement_. Both to be
// safe :)
void MemMang_Handler() {
  enter_dfu();
}

void MemoryManagement_Handler() {
  enter_dfu();
}

void UsageFault_Handler() {
  enter_dfu();
}

void BusFault_Handler() {
  enter_dfu();
}

void double_tapped() {
  if (!sys->getBacklight()->isOn()) {
    sys->getBacklight()->enableFor(1500);
  }
}


System::System() {
  // set up core system
  //Serial.begin(115200);
  sys = this;

  Wire.begin();
  SPI.begin();

  // set up watchdog
  // 9830401 = 5 minutes ((val-1)/32768) seconds
  // 3932161 = 2 minutes
  NRF_WDT->CONFIG = 0x01; // Configure WDT to run when CPU is asleep
  NRF_WDT->CRV = 3932161; // Timeout set to 120 seconds, timeout[s] = (CRV-1)/32768
  NRF_WDT->RREN = 0x01;     // Enable the RR[0] reload register
  NRF_WDT->TASKS_START = 1; // Start WDT

  // enable the DCDC converter
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

  // set up GPIOs

  pinMode(PIN_FLASHLIGHT, OUTPUT);
  digitalWrite(PIN_FLASHLIGHT, LOW);

  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, LOW);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  pinMode(PIN_LCD_DISP, OUTPUT);
  digitalWrite(PIN_LCD_DISP, HIGH);

  // attachInterrupt(PIN_SQW, doSomething, CHANGE);
  // i2cterm_setup();
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON3, INPUT_PULLUP);
  pinMode(PIN_BUTTON5, INPUT_PULLUP);

  pinMode(PIN_CHG, INPUT);
  pinMode(PIN_ACCELINT1, INPUT);

  commonSetup();
  //attachInterrupt(PIN_BUTTON3, i2cscan, FALLING);
  //attachInterrupt(PIN_BUTTON5, backlight_pin5, CHANGE);
  registerIRQ(PIN_ACCELINT1, double_tapped, RISING);
}
      

void System::registerIRQ(int pinnum, void (*fn)(), int mode) {
  attachInterrupt(pinnum, fn, mode);
}

bool System::getButtonPressed(int pin) {
  return (digitalRead(pin) != HIGH);
}

void System::feedWatchdog() { NRF_WDT->RR[0] = WDT_RR_RR_Reload; }

#endif
