#include "rtc.h"
#include "i2c.h"

#include <mutex>

using namespace rtc;

Rtc_Pcf8563 rtc::RTC;

RTC_DATA_ATTR bool rtc::initialized = false;

time_t compileTime()
{
  const time_t FUDGE(10);    //fudge factor to allow for upload time, etc. (seconds, YMMV)
  const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char compMon[3], *m;

  strncpy(compMon, compDate, 3);
  compMon[3] = '\0';
  m = strstr(months, compMon);

  tmElements_t tm;
  tm.Month = ((m - months) / 3 + 1);
  tm.Day = atoi(compDate + 4);
  tm.Year = atoi(compDate + 7) - 1970;
  tm.Hour = atoi(compTime);
  tm.Minute = atoi(compTime + 3);
  tm.Second = atoi(compTime + 6);



  time_t t = makeTime(tm);
  return t + FUDGE;        //add fudge factor to allow for compile time
}

void rtc::init() {
  std::lock_guard<std::mutex> guard = std::lock_guard(i2c::mutex);
  tmElements_t tm;
  breakTime(compileTime(), tm);
  RTC.setSquareWave(SQW_DISABLE);
  RTC.setDate(tm.Day, tm.Wday, tm.Month, 0, tmYearToY2k(tm.Year));
  RTC.setTime(tm.Hour, tm.Minute, tm.Second);
  resetAlarm();
  //RTC.alarmInterrupt(ALARM_2, true);
  //tmElements_t currentTime;
  //RTC.read(currentTime);
  //RTC.getTime();
  ESP_LOGI("RTC", "Time set to: %i:%i:%i\n", RTC.getHour(), RTC.getMinute(), RTC.getSecond());
}

void rtc::resetAlarm() {
  int nextAlarmMinute = 0;
  RTC.clearAlarm(); // resets the alarm flag in the RTC
  nextAlarmMinute = RTC.getMinute();
  nextAlarmMinute = 
  (nextAlarmMinute == 59)
  ? 0
  : (nextAlarmMinute + 1); // set alarm to trigger 1 minute from now
  RTC.setAlarm(nextAlarmMinute, 99, 99, 99);
}

void rtc::setTime(tmElements_t tm) {
  std::lock_guard<std::mutex> guard = std::lock_guard(i2c::mutex);

  //RTC.set(makeTime(newTime));
  RTC.setDate(tm.Day, tm.Wday, tm.Month, 0, tmYearToY2k(tm.Year));
  RTC.setTime(tm.Hour, tm.Minute, tm.Second);

  //RTC.getTime();
  ESP_LOGI("RTC", "Time set to: %i:%i:%i\n", RTC.getHour(), RTC.getMinute(), RTC.getSecond());
}

tmElements_t rtc::currentTime() {
  std::lock_guard<std::mutex> guard = std::lock_guard(i2c::mutex);

  //RTC.getDate();
  //RTC.getTime();
  tmElements_t now{};
  now.Year = RTC.getYear();
  now.Month = RTC.getMonth();
  now.Day = RTC.getDay();
  now.Wday = RTC.getWeekday();
  now.Hour = RTC.getHour();
  now.Minute = RTC.getMinute();
  now.Second = RTC.getSecond();
  return now;
}
/*
void rtc::resetAlarm() {
  std::lock_guard<std::mutex> guard = std::lock_guard(i2c::mutex);
  RTC.alarm(ALARM_2);
}
*/
