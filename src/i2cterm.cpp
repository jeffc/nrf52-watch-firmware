#include "Arduino.h"
#include <Wire.h>
 
void i2cterm_setup()
{
  Wire.begin();
 
  Serial.begin(115200);
  while (!Serial);             // Leonardo: wait for serial monitor
  Serial.println("\nI2C console");
  Serial.setTimeout(10000);
}
 

uint8_t address = 0x00;
 
void i2cterm_loop()
{
  Serial.print(address);
  Serial.print("> ");

  while (!Serial.available());

  char cmd = Serial.read();

  switch (cmd) {

    case 'a':
      address = Serial.parseInt();
      Serial.print("Set address to ");
      Serial.println(address);
      break;

    case 'w':
    {
      uint8_t data = Serial.parseInt();
      Wire.beginTransmission(address);
      Wire.write(data);
      Wire.endTransmission();
      Serial.print("Wrote ");
      Serial.println(data);
      break;
    }

    case 'r':
    {
      uint8_t amt = Serial.parseInt();
      Serial.print("Requesting ");
      Serial.print(amt);
      Serial.println(" bytes");
      Wire.requestFrom(address, amt);
      while(Wire.available()) {
        uint8_t b = Wire.read();
        Serial.print("0x");
        Serial.print(b, HEX);
        Serial.print("\n");
      }
      break;
    }

    default:
      Serial.print("Unknown cmd ");
      Serial.println(cmd);
      break;
  }

  while (Serial.available()) { Serial.read(); }
}

