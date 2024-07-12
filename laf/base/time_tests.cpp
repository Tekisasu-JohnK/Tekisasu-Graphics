// LAF Base Library
// Copyright (c) 2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/time.h"

using namespace base;

namespace base {

  std::ostream& operator<<(std::ostream& os, const Time& t) {
    return os << t.year << '-'
              << t.month << '-'
              << t.day << ' '
              << t.hour << ':'
              << t.minute << ':'
              << t.second;
  }

}

TEST(Time, Cmp)
{
  EXPECT_EQ(Time(2018, 5, 2, 0, 0, 1), Time(2018, 5, 2, 0, 0, 0).addSeconds(1));
  EXPECT_EQ(Time(2018, 5, 2, 0, 1, 0), Time(2018, 5, 2, 0, 0, 0).addMinutes(1));
  EXPECT_EQ(Time(2018, 5, 2, 1, 0, 0), Time(2018, 5, 2, 0, 0, 0).addHours(1));
  EXPECT_EQ(Time(2018, 5, 3, 0, 0, 0), Time(2018, 5, 2, 0, 0, 0).addDays(1));

  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2018, 5, 2, 0, 0, 1));
  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2018, 5, 2, 0, 1, 0));
  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2018, 5, 2, 1, 0, 0));
  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2018, 5, 3, 0, 0, 0));
  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2018, 6, 2, 0, 0, 0));
  EXPECT_TRUE(Time(2018, 5, 2, 0, 0, 0) < Time(2019, 5, 2, 0, 0, 0));

  EXPECT_FALSE(Time(2018, 5, 2, 0, 0, 1) < Time(2018, 5, 2, 0, 0, 0));
  EXPECT_FALSE(Time(2018, 5, 2, 0, 1, 0) < Time(2018, 5, 2, 0, 0, 0));
  EXPECT_FALSE(Time(2018, 5, 2, 1, 0, 0) < Time(2018, 5, 2, 0, 0, 0));
  EXPECT_FALSE(Time(2018, 5, 3, 0, 0, 0) < Time(2018, 5, 2, 0, 0, 0));
  EXPECT_FALSE(Time(2018, 6, 2, 0, 0, 0) < Time(2018, 5, 2, 0, 0, 0));
  EXPECT_FALSE(Time(2019, 5, 2, 0, 0, 0) < Time(2018, 5, 2, 0, 0, 0));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
