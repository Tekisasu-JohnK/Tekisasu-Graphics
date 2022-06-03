// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_TIME_H_INCLUDED
#define BASE_TIME_H_INCLUDED
#pragma once

#include "base/ints.h"

#include <ctime>
#include <string>

namespace base {

  // ~1000 per second
  typedef uint64_t tick_t;

  class Time {
  public:
    int year, month, day;
    int hour, minute, second;

    Time(int year = 0, int month = 0, int day = 0,
         int hour = 0, int minute = 0, int second = 0)
      : year(year), month(month), day(day)
      , hour(hour), minute(minute), second(second) {
    }

    bool valid() const {
      return (year != 0 && month != 0 && day != 0);
    }

    void dateOnly() {
      hour = minute = second = 0;
    }

    Time& addSeconds(const int seconds);
    Time& addMinutes(const int minutes) {
      return addSeconds(minutes*60);
    }
    Time& addHours(const int hours) {
      return addSeconds(hours*60*60);
    }
    Time& addDays(const int days) {
      return addSeconds(days*24*60*60);
    }

    bool operator==(const Time& other) const {
      return
        year == other.year &&
        month == other.month &&
        day == other.day &&
        hour == other.hour &&
        minute == other.minute &&
        second == other.second;
    }

    bool operator!=(const Time& other) const {
      return !operator==(other);
    }

    bool operator<(const Time& other) const;
  };

  bool safe_localtime(const std::time_t time, std::tm* result);

  Time current_time();
  tick_t current_tick();

}

#endif
