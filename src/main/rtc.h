#pragma once

//#include <DS3232RTC.h>
#include <Rtc_Pcf8563.h>
#include <TimeLib.h>

namespace rtc {
  //extern DS3232RTC RTC; This is shitty but I haven't made an elegent solution yet. Comment the RTC you don't want
  
  extern Rtc_Pcf8563 RTC;

  extern bool initialized;

  void init();

  void setTime(tmElements_t newTime);

  tmElements_t currentTime();

  void resetAlarm();
}
