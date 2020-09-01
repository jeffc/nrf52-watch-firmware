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
  Wire.write((uint8_t)((val >> 8) & 0xFF));
  Wire.write((uint8_t)(val & 0xFF));
  Wire.endTransmission();
}


void battery_setup() {
  uint16_t status_reg = i2c_read_register16(0x36, 0x00);

  Serial.printf("0x%x\r\n", status_reg);

  if (status_reg & 0x02) { // power on reset (POR) bit is set
    // do setup
    
    uint16_t fstat_reg = i2c_read_register16(0x36, 0x3D);
    if (fstat_reg & 1) { // DNR
      Serial.println("DNR flag set, skipping");
    } else {

      // get hibcfg value
      uint16_t hibcfg = i2c_read_register16(0x36, 0xBA);
      // wake up (3 steps, per manufacturer reference)
      i2c_write_register16(0x36, 0x60, 0x0090);
      i2c_write_register16(0x36, 0xBA, 0x0000);
      i2c_write_register16(0x36, 0x60, 0x0000);

      // designcap
      i2c_write_register16(0x36, 0x18, 1000); // 0.5mAh resolution * 500mAh = 1000

      // IchgTerm ; according to datasheet, this is 7.5% of Ireg, which is
      // 1000V/Rreg, which is 500mA, so IchgTerm = 37.5mA. At 156.25 uA
      // resolution, this gives a register value of 240
      i2c_write_register16(0x36, 0x1E, 240);

      // Vempty. 2.8V / 1.25mV resolution = 2240
      i2c_write_register16(0x36, 0x3A, 2240);

      // write model
      i2c_write_register16(0x36, 0xDB, 0x8000);
      
      i2c_write_register16(0x36, 0xBA, hibcfg);


      // clear POR bit
      Serial.println("Clearing POR flag");
      while (i2c_read_register16(0x36, 0x00) & 0x02) { i2c_write_register16(0x36, 0x00, status_reg & 0xFFFD); }
    }
  } else {
    uint16_t battery_pct = i2c_read_register16(0x36, 0x06);
    uint8_t bpct_l = battery_pct & 0xFF;
    uint8_t bpct_h = (battery_pct >> 8) & 0xFF;
    Serial.printf("%d %d (0x%x 0x%x)\r\n", bpct_h, bpct_l, bpct_h, bpct_l);
  }

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
  pinMode(PIN_BUTTON2, INPUT_PULLUP);
  pinMode(PIN_BUTTON3, INPUT_PULLUP);
  pinMode(PIN_BUTTON4, INPUT_PULLUP);
  pinMode(PIN_BUTTON5, INPUT_PULLUP);

  pinMode(PIN_CHG, INPUT_PULLUP);

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
  gfx.printf("%02d%c%02d\n", now.hour(), ((now.second() % 2) ? ':' : ' '), now.minute());


  gfx.refresh();
	delay(500);

  if (!digitalRead(PIN_BUTTON3)) {
    battery_setup();
  }
}
