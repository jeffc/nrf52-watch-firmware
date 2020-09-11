#ifdef EMBEDDED

#include "rtc.h"
#include <M41T62.h>

RTC_M41T62 *_rtc;

RTC::RTC() {
  _rtc = new RTC_M41T62();
  _rtc->begin();

  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  _rtc->writeSqwPinMode(Sqw1Hz);
  _rtc->checkFlags();
  _rtc->alarmEnable(0);
}

RTCDateTime RTC::now() {
  DateTime now = _rtc->now();
  RTCDateTime out = {.year = now.year(),
                     .month = now.month(),
                     .day = now.day(),
                     .hour = now.hour(),
                     .minute = now.minute(),
                     .second = now.second(),
                     .dayOfWeek = now.dayOfWeek()};
  return out;
}

void RTC::set_unixt(int ut) { _rtc->adjust(ut); }

#endif
