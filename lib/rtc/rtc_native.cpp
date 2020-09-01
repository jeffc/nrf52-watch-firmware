#ifdef NATIVE

#include <stdio.h>
#include <ctime>
#include "rtc.h"

void rtc_init() {
  printf("rtc_init() called\n");
}

RTCDateTime rtc_now() {
  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);
  return RTCDateTime {
    .year = (uint16_t)now->tm_year,
    .month = (uint8_t)now->tm_mon,
    .day = (uint8_t)now->tm_mday,
    .hour = (uint8_t)now->tm_hour,
    .minute = (uint8_t)now->tm_min,
    .second = (uint8_t)now->tm_sec,
    .dayOfWeek = (uint8_t)now->tm_wday
  };
}



#endif
