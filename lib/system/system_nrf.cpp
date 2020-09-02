#ifdef EMBEDDED

#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "util.h"
#include "system.h"
#include "battery.h"

// todo: move to backlight module
void backlight_pin5() {
  if (digitalRead(PIN_BUTTON5)) {
    digitalWrite(PIN_BACKLIGHT, LOW);
  } else {
    digitalWrite(PIN_BACKLIGHT, HIGH);
  }
}

void init_peripherals() {
  // set up core system
	Serial.begin(115200);
	Wire.begin();


  // set up GPIOs

  pinMode(PIN_FLASHLIGHT, OUTPUT);
  digitalWrite(PIN_FLASHLIGHT, LOW);

  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, LOW);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  //attachInterrupt(PIN_SQW, doSomething, CHANGE);
  //i2cterm_setup();
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);
  pinMode(PIN_BUTTON3, INPUT_PULLUP);
  pinMode(PIN_BUTTON4, INPUT_PULLUP);
  pinMode(PIN_BUTTON5, INPUT_PULLUP);

  pinMode(PIN_CHG, INPUT_PULLUP);

  attachInterrupt(PIN_BUTTON1, enter_dfu, CHANGE);

  attachInterrupt(PIN_BUTTON3, i2cscan, FALLING);
  attachInterrupt(PIN_BUTTON5, backlight_pin5, CHANGE);
}

#endif
