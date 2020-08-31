#include "Arduino.h"
#include <Wire.h>
#include <nrfx_spi.h>


#include "pins.h"

#include "M41T62.h"

#include "Adafruit_SharpMem.h"
#include <Fonts/FreeMonoBold24pt7b.h>

#define DFU_MAGIC_UF2_RESET             0x57

extern void i2cterm_setup();
extern void i2cterm_loop();

RTC_M41T62 rtc;

// 240 accounts for 10 imaginary pixels at the end of each row
Adafruit_SharpMem gfx = Adafruit_SharpMem(PIN_SPI_SCK, PIN_SPI_MOSI, PIN_LCD_CS, 240, 303);

void i2cscan() {
	byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for(address = 1; address < 127; address++ )
	{
		// The i2c_scanner uses the return value of
		// the Write.endTransmisstion to see if
		// a device did acknowledge to the address.
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.print(address,HEX);
			Serial.println("  !");

			nDevices++;
		}
		else if (error==4)
		{
			Serial.print("Unknown error at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.println(address,HEX);
		}    
	}
	if (nDevices == 0)
		Serial.println("No I2C devices found\n");
	else
		Serial.println("done\n");
}

void doSomething() {
  digitalToggle(PIN_FLASHLIGHT);
}

void enter_dfu() {
  NRF_POWER->GPREGRET = DFU_MAGIC_UF2_RESET;
  NVIC_SystemReset();
}

void setup() {
	//i2cterm_setup();
	Serial.begin(115200);
	Wire.begin();
	rtc.begin();
  //nrfx_spi_uninit();

	// following line sets the RTC to the date & time this sketch was compiled
	//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	// This line sets the RTC with an explicit date & time, for example to set
	// January 21, 2014 at 3am you would call:
	// rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  rtc.writeSqwPinMode(Sqw1Hz);
  rtc.checkFlags();
  rtc.alarmEnable(0);

  gfx.begin();

  gfx.clearDisplay();
  gfx.setRotation(2);
  gfx.setFont(&FreeMonoBold24pt7b);
  gfx.refresh();

  pinMode(PIN_FLASHLIGHT, OUTPUT);
  digitalWrite(PIN_FLASHLIGHT, LOW);

  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, LOW);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  //attachInterrupt(PIN_SQW, doSomething, CHANGE);
  //i2cterm_setup();
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, OUTPUT);
  pinMode(PIN_BUTTON3, INPUT_PULLUP);
  pinMode(PIN_BUTTON4, OUTPUT);
  pinMode(PIN_BUTTON5, INPUT_PULLUP);

  digitalWrite(PIN_BUTTON2, LOW);
  digitalWrite(PIN_BUTTON4, LOW);

  attachInterrupt(PIN_BUTTON1, enter_dfu, CHANGE);
}

void loop() {
  //Wire.beginTransmission(0x36);
  //Wire.write(0x0A);
  //Wire.endTransmission();

  //Wire.requestFrom(0x36, 2);

  //int8_t w1 = Wire.read();
  //int8_t w2 = Wire.read();
  //

  ////double i = (w * 156.25)/1000.; 
  //Serial.printf("%d\r\n",(w2<<8)|w1);
  //delay(100);
  //int a = digitalRead(11);
  //int b = digitalRead(12);
  //int c = digitalRead(13);
  //Serial.printf("%d %d %d\r\n", a, b, c);
  //i2cterm_loop();
  //suspendLoop();
	//i2cterm_loop();

  //i2cscan();

	DateTime now = rtc.now();

  gfx.clearBuffer();

  gfx.setCursor(30, 50);
  gfx.setTextColor(0);

	//gfx.print(now.year(), DEC);
	//gfx.print('/');
	//gfx.print(now.month(), DEC);
	//gfx.print('/');
	//gfx.print(now.day(), DEC);
	//gfx.print(' ');
	//gfx.print(now.hour(), DEC);
	//gfx.print(':');
	//gfx.print(now.minute(), DEC);
	//gfx.print(':');
	//gfx.print(now.second(), DEC);
	//gfx.println();
  gfx.printf("%02d:%02d\n", now.hour(), now.minute());


  gfx.refresh();

	//Serial.print(" since midnight 1/1/1970 = ");
	//Serial.print(now.unixtime());
	//Serial.print("s = ");
	//Serial.print(now.unixtime() / 86400L);
	//Serial.println("d");

	//// calculate a date which is 7 days and 30 seconds into the future
	//DateTime future (now.unixtime() + 7 * 86400L + 30);

	//Serial.print(" now + 7d + 30s: ");
	//Serial.print(future.year(), DEC);
	//Serial.print('/');
	//Serial.print(future.month(), DEC);
	//Serial.print('/');
	//Serial.print(future.day(), DEC);
	//Serial.print(' ');
	//Serial.print(future.hour(), DEC);
	//Serial.print(':');
	//Serial.print(future.minute(), DEC);
	//Serial.print(':');
	//Serial.print(future.second(), DEC);
	//Serial.println();

	//Serial.println();
	delay(500);
}
