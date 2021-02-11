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

// https://stackoverflow.com/a/5590518
bool isDST(int month, int day, int dow) {
	//January, february, and december are out.
	if (month < 3 || month > 11) { return false; }
	//April to October are in
	if (month > 3 && month < 11) { return true; }
	int previousSunday = day - dow;
	//In march, we are DST if our previous sunday was on or after the 8th.
	if (month == 3) { return previousSunday >= 8; }
	//In november we must be before the first sunday to be dst.
	//That means the previous sunday must be before the 1st.
	return previousSunday <= 0;
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
