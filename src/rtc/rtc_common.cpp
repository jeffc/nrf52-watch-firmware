#include "rtc.h"

int tz_offset = -4;

void RTC::set_tz_offset(int offset) {
  tz_offset = offset;
}

int RTC::get_tz_offset() {
  return tz_offset;
}
