// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#include "M41T62.h"
#include <Wire.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#define WIRE Wire
#else
#define PROGMEM
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define WIRE Wire1
#endif

/* M41T62 Register Map
 * http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00019860.pdf
 */
#define M41T62_ADDRESS 0x68   // I2C Address
#define M41T62_TNTH_SEC 0x00  // 10ths/100ths Seconds
#define M41T62_SEC 0x01       // Seconds
#define M41T62_MIN 0x02       // Minutes
#define M41T62_HRS 0x03       // Hours
#define M41T62_SQWFQ_DOW 0x04 // SQW Frequency / Day of Week
#define M41T62_DAY 0x05       // Day of Month
#define M41T62_MON 0x06       // Month
#define M41T62_YRS 0x07       // Year
#define M41T62_CAL 0x08       // Calibration
#define M41T62_WDOG 0x09      // Watchdog
#define M41T62_SQWEN_AMO 0x0A // SQW Enable / Alarm Month
#define M41T62_ADOM 0x0B      // Alarm Day of Month
#define M41T62_AHRS 0x0C      // Alarm Hour
#define M41T62_AMIN 0x0D      // Alarm Minutes
#define M41T62_ASEC 0x0E      // Alarm Seconds
#define M41T62_FLAGS 0x0F     // Flags: WDF | AF | 0 | 0 | 0 | OF | 0 | 0

#define SECONDS_PER_DAY 86400L
#define SECONDS_FROM_1970_TO_2000 946684800

#if (ARDUINO >= 100)
#include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
// Macro to deal with the difference in I2C write functions from old and new
// Arduino versions.
#define _I2C_WRITE write
#define _I2C_READ read
#else
#include <WProgram.h>
#define _I2C_WRITE send
#define _I2C_READ receive
#endif

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

const uint8_t daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30,
                                       31, 31, 30, 31, 30, 31};

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
  if (y >= 2000)
    y -= 2000;
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)
    days += pgm_read_byte(daysInMonth + i - 1);
  if (m > 2 && y % 4 == 0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime(uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000; // bring to 2000 timestamp from 1970

  ss = t % 60;
  t /= 60;
  mm = t % 60;
  t /= 60;
  hh = t % 24;
  uint16_t days = t / 24;
  uint8_t leap;
  for (yOff = 0;; ++yOff) {
    leap = yOff % 4 == 0;
    if (days < 365 + leap)
      break;
    days -= 365 + leap;
  }
  for (m = 1;; ++m) {
    uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
    if (leap && m == 2)
      ++daysPerMonth;
    if (days < daysPerMonth)
      break;
    days -= daysPerMonth;
  }
  d = days + 1;
}

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                   uint8_t min, uint8_t sec) {
  if (year >= 2000)
    year -= 2000;
  yOff = year;
  m = month;
  d = day;
  hh = hour;
  mm = min;
  ss = sec;
}

DateTime::DateTime(const DateTime &copy)
    : yOff(copy.yOff), m(copy.m), d(copy.d), hh(copy.hh), mm(copy.mm),
      ss(copy.ss) {}

static uint8_t conv2d(const char *p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using F() would further reduce the RAM footprint, see below.
DateTime::DateTime(const char *date, const char *time) {
  // sample input: date = "Dec 26 2009", time = "12:34:56"
  yOff = conv2d(date + 9);
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch (date[0]) {
  case 'J':
    m = date[1] == 'a' ? 1 : date[2] == 'n' ? 6 : 7;
    break;
  case 'F':
    m = 2;
    break;
  case 'A':
    m = date[2] == 'r' ? 4 : 8;
    break;
  case 'M':
    m = date[2] == 'r' ? 3 : 5;
    break;
  case 'S':
    m = 9;
    break;
  case 'O':
    m = 10;
    break;
  case 'N':
    m = 11;
    break;
  case 'D':
    m = 12;
    break;
  }
  d = conv2d(date + 4);
  hh = conv2d(time);
  mm = conv2d(time + 3);
  ss = conv2d(time + 6);
}

// A convenient constructor for using "the compiler's time":
// This version will save RAM by using PROGMEM to store it by using the F macro.
//   DateTime now (F(__DATE__), F(__TIME__));
DateTime::DateTime(const __FlashStringHelper *date,
                   const __FlashStringHelper *time) {
  // sample input: date = "Dec 26 2009", time = "12:34:56"
  char buff[11];
  memcpy_P(buff, date, 11);
  yOff = conv2d(buff + 9);
  // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
  switch (buff[0]) {
  case 'J':
    m = buff[1] == 'a' ? 1 : m = buff[2] == 'n' ? 6 : 7;
    break;
  case 'F':
    m = 2;
    break;
  case 'A':
    m = buff[2] == 'r' ? 4 : 8;
    break;
  case 'M':
    m = buff[2] == 'r' ? 3 : 5;
    break;
  case 'S':
    m = 9;
    break;
  case 'O':
    m = 10;
    break;
  case 'N':
    m = 11;
    break;
  case 'D':
    m = 12;
    break;
  }
  d = conv2d(buff + 4);
  memcpy_P(buff, time, 8);
  hh = conv2d(buff);
  mm = conv2d(buff + 3);
  ss = conv2d(buff + 6);
}

uint8_t DateTime::dayOfWeek() const {
  uint16_t day = date2days(yOff, m, d);
  return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

uint32_t DateTime::unixtime(void) const {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

  return t;
}

long DateTime::secondstime(void) const {
  long t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  return t;
}

DateTime DateTime::operator+(const TimeSpan &span) {
  return DateTime(unixtime() + span.totalseconds());
}

DateTime DateTime::operator-(const TimeSpan &span) {
  return DateTime(unixtime() - span.totalseconds());
}

TimeSpan DateTime::operator-(const DateTime &right) {
  return TimeSpan(unixtime() - right.unixtime());
}

////////////////////////////////////////////////////////////////////////////////
// TimeSpan implementation

TimeSpan::TimeSpan(int32_t seconds) : _seconds(seconds) {}

TimeSpan::TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds)
    : _seconds(days * 86400L + hours * 3600 + minutes * 60 + seconds) {}

TimeSpan::TimeSpan(const TimeSpan &copy) : _seconds(copy._seconds) {}

TimeSpan TimeSpan::operator+(const TimeSpan &right) {
  return TimeSpan(_seconds + right._seconds);
}

TimeSpan TimeSpan::operator-(const TimeSpan &right) {
  return TimeSpan(_seconds - right._seconds);
}

////////////////////////////////////////////////////////////////////////////////
// RTC_M41T62 implementation

static uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

uint8_t RTC_M41T62::begin(void) {
  /*  The M41T62 "OUT" bit should be set to 1 (HIGH) by default when
   *  used with this library. This keeps the interrupt pin high
   *  even if alarm, watchdog and oscillator bits are not enabled.
   *  Interrupts are active-low. Default OUT bit is 0 so we set it
   *  to 1 here.
   */
  int currentByte;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_CAL);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);
  currentByte = WIRE._I2C_READ();

  if (!bitRead(currentByte, 7)) {
    bitWrite(currentByte, 7, 1);
    WIRE.beginTransmission(M41T62_ADDRESS);
    WIRE._I2C_WRITE((uint8_t)M41T62_CAL);
    WIRE._I2C_WRITE((uint8_t)currentByte);
    WIRE.endTransmission();
  }
  return 1;
}

void RTC_M41T62::adjust(const DateTime &dt) {
  int dow;
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWFQ_DOW);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);
  dow = WIRE._I2C_READ();

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)1);
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.second()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.minute()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.hour()));
  WIRE._I2C_WRITE((uint8_t)dow);
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.day()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.month()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.year() - 2000));
  WIRE.endTransmission();
}

DateTime RTC_M41T62::now() {
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)1);
  WIRE.endTransmission();

  WIRE.requestFrom(M41T62_ADDRESS, 7, 1); // 7
  uint8_t ss = bcd2bin(WIRE._I2C_READ() & 0x7F);
  uint8_t mm = bcd2bin(WIRE._I2C_READ() & 0x7F);
  uint8_t hh = bcd2bin(WIRE._I2C_READ() & 0x3F);
  WIRE._I2C_READ(); // Skip day of week
  uint8_t d = bcd2bin(WIRE._I2C_READ() & 0x3F);
  uint8_t m = bcd2bin(WIRE._I2C_READ() & 0x1F);
  uint16_t y = bcd2bin(WIRE._I2C_READ()) + 2000;
  return DateTime(y, m, d, hh, mm, ss);
}

M41T62SqwPinMode RTC_M41T62::readSqwPinMode() {
  int mode;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWFQ_DOW);
  WIRE.endTransmission();

  WIRE.requestFrom(M41T62_ADDRESS, 1);
  mode = WIRE._I2C_READ() >> 4;
  return static_cast<M41T62SqwPinMode>(mode);
}

void RTC_M41T62::writeSqwPinMode(M41T62SqwPinMode mode) {
  int currentByte;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWFQ_DOW);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);
  currentByte = WIRE._I2C_READ();

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWFQ_DOW);
  WIRE._I2C_WRITE((uint8_t)(currentByte & 0x07) | (mode << 4));
  WIRE.endTransmission();

  // Flip SQW Enable bit in register M41T62_SQWEN_AMO
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWEN_AMO);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);

  if (mode == 0) { // Disable
    bitWrite(currentByte, 6, 0);
  } else { // Enable
    bitWrite(currentByte, 6, 1);
  }

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWEN_AMO);
  WIRE._I2C_WRITE((uint8_t)currentByte);
  WIRE.endTransmission();
}

void RTC_M41T62::alarmEnable(bool onOff) {
  int currentByte;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWEN_AMO);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);
  currentByte = WIRE._I2C_READ();

  if (onOff) {
    bitWrite(currentByte, 7, 1);
  } else {
    bitWrite(currentByte, 7, 0);
  }

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWEN_AMO);
  WIRE._I2C_WRITE((uint8_t)currentByte);
  WIRE.endTransmission();
}

void RTC_M41T62::alarmRepeat(int mode) { // set alarm repeat mode
  int byte1, byte2, byte3, byte4;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_ADOM);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 4);
  byte1 = WIRE._I2C_READ();
  byte2 = WIRE._I2C_READ();
  byte3 = WIRE._I2C_READ();
  byte4 = WIRE._I2C_READ();

  switch (mode) {
  case 1: // once per second
    bitWrite(byte4, 7, 1);
    bitWrite(byte3, 7, 1);
    bitWrite(byte2, 7, 1);
    bitWrite(byte1, 7, 1);
    bitWrite(byte1, 6, 1);
    break;
  case 2: // once per minute
    bitWrite(byte4, 7, 0);
    bitWrite(byte3, 7, 1);
    bitWrite(byte2, 7, 1);
    bitWrite(byte1, 7, 1);
    bitWrite(byte1, 6, 1);
    break;
  case 3: // once per hour
    bitWrite(byte4, 7, 0);
    bitWrite(byte3, 7, 0);
    bitWrite(byte2, 7, 1);
    bitWrite(byte1, 7, 1);
    bitWrite(byte1, 6, 1);
    break;
  case 4: // once per day
    bitWrite(byte4, 7, 0);
    bitWrite(byte3, 7, 0);
    bitWrite(byte2, 7, 0);
    bitWrite(byte1, 7, 1);
    bitWrite(byte1, 6, 1);
    break;
  case 5: // once per month
    bitWrite(byte4, 7, 0);
    bitWrite(byte3, 7, 0);
    bitWrite(byte2, 7, 0);
    bitWrite(byte1, 7, 0);
    bitWrite(byte1, 6, 1);
    break;
  case 6: // once per year
    bitWrite(byte4, 7, 0);
    bitWrite(byte3, 7, 0);
    bitWrite(byte2, 7, 0);
    bitWrite(byte1, 7, 0);
    bitWrite(byte1, 6, 0);
    break;
  }

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_ADOM);
  WIRE._I2C_WRITE((uint8_t)byte1);
  WIRE._I2C_WRITE((uint8_t)byte2);
  WIRE._I2C_WRITE((uint8_t)byte3);
  WIRE._I2C_WRITE((uint8_t)byte4);
  WIRE.endTransmission();
  pointerReset();
}

int RTC_M41T62::alarmRepeat() { // return alarm repeat mode
  int byte1, byte2, byte3, byte4, mode = 0, retVal = 0;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_ADOM);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 4);
  byte1 = WIRE._I2C_READ();
  byte2 = WIRE._I2C_READ();
  byte3 = WIRE._I2C_READ();
  byte4 = WIRE._I2C_READ();
  bitWrite(mode, 0, bitRead(byte4, 7));
  bitWrite(mode, 1, bitRead(byte3, 7));
  bitWrite(mode, 2, bitRead(byte2, 7));
  bitWrite(mode, 3, bitRead(byte1, 7));
  bitWrite(mode, 4, bitRead(byte1, 6));

  switch (mode) {
  case 31:
    retVal = 1;
    break; // once per second
  case 30:
    retVal = 2;
    break; // once per minute
  case 28:
    retVal = 3;
    break; // once per hour
  case 24:
    retVal = 4;
    break; // once per day
  case 16:
    retVal = 5;
    break; // once per month
  case 0:
    retVal = 6;
    break; // once per year
  default:
    retVal = 0;
    break; // ERROR
  }
  pointerReset();
  return retVal;
}

void RTC_M41T62::alarmSet(const DateTime &dt) {
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_SQWEN_AMO);
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.month()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.day()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.hour()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.minute()));
  WIRE._I2C_WRITE((uint8_t)bin2bcd(dt.second()));
  WIRE._I2C_WRITE((uint8_t)0);
  WIRE.endTransmission();
}

int RTC_M41T62::checkFlags() {
  // Returns 1 if Alarm flag is set, 0 if not. Ignores watchdog & oscillator
  // fail
  int byte1;

  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)M41T62_FLAGS);
  WIRE.endTransmission();
  WIRE.requestFrom(M41T62_ADDRESS, 1);
  byte1 = WIRE._I2C_READ();
  if (bitRead(byte1, 6) == 1) {
    return 1;
  } else {
    return 0;
  }
}

void RTC_M41T62::pointerReset() {
  // reset address pointer to 0 per datasheet note pg23
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)0);
  WIRE.endTransmission();
}

////////////////////////////////////////////////////////////////////////////////
// RTC_Millis implementation

long RTC_Millis::offset = 0;

void RTC_Millis::adjust(const DateTime &dt) {
  offset = dt.unixtime() - millis() / 1000;
}

DateTime RTC_Millis::now() { return (uint32_t)(offset + millis() / 1000); }

////////////////////////////////////////////////////////////////////////////////
// Functions for testing only

void RTC_M41T62::printAllBits() {
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)0);
  WIRE.endTransmission();

  WIRE.requestFrom(M41T62_ADDRESS, 16);
  for (int regCount = 0; regCount < 16; regCount++) {
    printBits(WIRE._I2C_READ());
    Serial.print(" : 0x");
    Serial.println(regCount, HEX);
  }
  // reset address pointer to 0 per datasheet note pg23
  WIRE.beginTransmission(M41T62_ADDRESS);
  WIRE._I2C_WRITE((uint8_t)0);
  WIRE.endTransmission();
}

void RTC_M41T62::printBits(byte myByte) {
  for (byte mask = 0x80; mask; mask >>= 1) {
    if (mask & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}
