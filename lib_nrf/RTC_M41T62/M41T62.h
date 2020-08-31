// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!

#ifndef _RTCLIB_H_
#define _RTCLIB_H_

#include <Arduino.h>

class TimeSpan;

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0);
    DateTime (const DateTime& copy);
    DateTime (const char* date, const char* time);
    DateTime (const __FlashStringHelper* date, const __FlashStringHelper* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

    DateTime operator+(const TimeSpan& span);
    DateTime operator-(const TimeSpan& span);
    TimeSpan operator-(const DateTime& right);

protected:
    uint8_t yOff, m, d, hh, mm, ss;
};

// Timespan which can represent changes in time with seconds accuracy.
class TimeSpan {
public:
    TimeSpan (int32_t seconds = 0);
    TimeSpan (int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
    TimeSpan (const TimeSpan& copy);
    int16_t days() const         { return _seconds / 86400L; }
    int8_t  hours() const        { return _seconds / 3600 % 24; }
    int8_t  minutes() const      { return _seconds / 60 % 60; }
    int8_t  seconds() const      { return _seconds % 60; }
    int32_t totalseconds() const { return _seconds; }

    TimeSpan operator+(const TimeSpan& right);
    TimeSpan operator-(const TimeSpan& right);

protected:
    int32_t _seconds;
};

// RTC based on the M41T62 chip connected via I2C and the Wire library
enum M41T62SqwPinMode { SqwNONE = 0x00, Sqw1Hz = 0x0F, Sqw2Hz = 0x0E, Sqw4Hz = 0x0D, Sqw8Hz = 0x0C,
    Sqw16Hz = 0x0B, Sqw32Hz = 0x0A, Sqw64Hz = 0x09, Sqw128Hz = 0x08, Sqw256Hz = 0x07, Sqw512Hz = 0x06,
    Sqw1kHz = 0x05, Sqw2kHz = 0x04, Sqw4kHz = 0x03, Sqw8kHz = 0x02, Sqw32kHz = 0x01 };

class RTC_M41T62 {
public:
    static uint8_t begin(void);
    static void adjust(const DateTime& dt);
    static DateTime now();
    static M41T62SqwPinMode readSqwPinMode();
    static void writeSqwPinMode(M41T62SqwPinMode mode);
    void alarmEnable(bool onOff);
    void alarmRepeat(int mode);
    int alarmRepeat();
    static void alarmSet(const DateTime& dt);
    int checkFlags();

    // Functions for testing only:
    static void printAllBits();
    static void printBits(byte myByte);

private:
    static void pointerReset();
};

// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
    static void begin(const DateTime& dt) { adjust(dt); }
    static void adjust(const DateTime& dt);
    static DateTime now();

protected:
    static long offset;
};

#endif // _RTCLIB_H_
