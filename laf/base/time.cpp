// LAF Base Library
// Copyright (c) 2021-2022 Igara Studio S.A.
// Copyright (c) 2001-2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/time.h"

#if LAF_WINDOWS
  #include <windows.h>
#else
  #include <sys/time.h>
  #if __APPLE__
    #include <mach/mach_time.h>
  #endif
#endif

namespace base {

bool safe_localtime(const std::time_t time, std::tm* result)
{
#if LAF_WINDOWS
  // localtime_s returns errno_t == 0 if there is no error
  return (localtime_s(result, &time) != 0);
#else
  return (localtime_r(&time, result) != nullptr);
#endif
}

Time current_time()
{
#if LAF_WINDOWS

  SYSTEMTIME st;
  GetLocalTime(&st);
  return Time(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

#else

  std::time_t now = std::time(nullptr);
  std::tm t;
  safe_localtime(now, &t);
  return Time(
    t.tm_year+1900, t.tm_mon+1, t.tm_mday,
    t.tm_hour, t.tm_min, t.tm_sec);

#endif
}

tick_t current_tick()
{
#if LAF_WINDOWS
  // GetTickCount() is limited to the system timer resolution (from 10
  // to 16 milliseconds), we prefer QueryPerformanceCounter().
  LARGE_INTEGER counter, freq;
  if (QueryPerformanceCounter(&counter) &&
      // TODO Call QueryPerformanceFrequency() just one time
      QueryPerformanceFrequency(&freq)) {
    // TODO Some precision is lost, we could return float or double
    return counter.QuadPart * 1000 / freq.QuadPart;
  }
  else
    return GetTickCount();
#elif __APPLE__
  static mach_timebase_info_data_t timebase = { 0, 0 };
  if (timebase.denom == 0)
    (void)mach_timebase_info(&timebase);
  return tick_t(double(mach_absolute_time()) * double(timebase.numer) / double(timebase.denom) / 1.0e6);
#else
  // TODO use clock_gettime(CLOCK_MONOTONIC, &now); if it's possible
  struct timeval now;
  gettimeofday(&now, nullptr);
  return now.tv_sec*1000 + now.tv_usec/1000;
#endif
}

Time& Time::addSeconds(const int seconds)
{
  struct std::tm tm;
  tm.tm_sec = second;
  tm.tm_min = minute;
  tm.tm_hour = hour;
  tm.tm_mday = day;
  tm.tm_mon = month-1;
  tm.tm_year = year-1900;
  tm.tm_wday = 0;
  tm.tm_yday = 0;

  // The value is negative if no information is available about
  // Daylight Saving Time (DST). If we use 0 we might get one hour of
  // difference between the input and the output.
  tm.tm_isdst = -1;

  std::time_t tt = std::mktime(&tm);

  tt += seconds;

  std::tm t;
  safe_localtime(tt, &t);

  year = t.tm_year+1900;
  month = t.tm_mon+1;
  day = t.tm_mday;
  hour = t.tm_hour;
  minute = t.tm_min;
  second = t.tm_sec;

  return *this;
}

bool Time::operator<(const Time& other) const
{
  int d = year - other.year;
  if (d != 0) return (d < 0);

  d = month - other.month;
  if (d != 0) return (d < 0);

  d = day - other.day;
  if (d != 0) return (d < 0);

  d = hour - other.hour;
  if (d != 0) return (d < 0);

  d = minute - other.minute;
  if (d != 0) return (d < 0);

  return (second < other.second);
}

} // namespace base
