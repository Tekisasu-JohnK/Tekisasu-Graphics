// LAF Base Library
// Copyright (c) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/pi.h"

using namespace base;

TEST(Pi, FModRadians)
{
  EXPECT_DOUBLE_EQ(0.0, fmod_radians(0.0));
  EXPECT_DOUBLE_EQ(0.0, fmod_radians(2*PI));
  EXPECT_DOUBLE_EQ(PI, fmod_radians(PI));
  EXPECT_DOUBLE_EQ(-PI, fmod_radians(-PI));
  EXPECT_DOUBLE_EQ(-3*PI/4, fmod_radians(5*PI/4));
  EXPECT_DOUBLE_EQ(-3*PI/4, fmod_radians(5*PI/4 + 2*PI));
  EXPECT_DOUBLE_EQ(PI/2, fmod_radians(10*PI/4));

  for (double a=-100*PI; a<=100*PI; a+=PI/4) {
    double b = fmod_radians(a);
    EXPECT_GE(b, -PI-1e-10) << a << " radians, " << (180.0 * a / PI) << " degrees";
    EXPECT_LE(b, +PI+1e-10) << a << " radians, " << (180.0 * a / PI) << " degrees";
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
