/**
 * Abstraction layer around the RTC_M41T62 library
 */
#ifndef _RTC_H_
#define _RTC_H_

#include <stdint.h>

typedef struct {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t dayOfWeek;
} RTCDateTime;

class RTC {
  public:
    RTC();
    void set_unixt(int ut);
    RTCDateTime now();
};

#endif
