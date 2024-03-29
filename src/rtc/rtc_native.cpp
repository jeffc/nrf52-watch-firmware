#ifdef NATIVE

#include "rtc.h"
#include <ctime>
#include <stdio.h>

RTC::RTC() { printf("rtc_init() called\n"); }

RTCDateTime RTC::now() {
  std::time_t t = std::time(0);

  t = (time_t)(t + (get_tz_offset() * 3600));

  std::tm *now = std::gmtime(&t);
  return RTCDateTime{.year = (uint16_t)now->tm_year +
                             1900 /* to match the physical RTC interface */,
                     .month =
                         (uint8_t)now->tm_mon + 1 /* library is 1-indexed */,
                     .day = (uint8_t)now->tm_mday,
                     .hour = (uint8_t)now->tm_hour,
                     .minute = (uint8_t)now->tm_min,
                     .second = (uint8_t)now->tm_sec,
                     .dayOfWeek = (uint8_t)now->tm_wday};
}

void RTC::set_unixt(int ut) { printf("Called set_unixt(%d)\n", ut); }

#endif
