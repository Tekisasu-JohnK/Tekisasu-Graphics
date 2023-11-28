// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/ints.h"
#include "base/count_bits.h"

using namespace base;

TEST(CountBits, CommonCases)
{
  EXPECT_EQ(0, count_bits(0));
  EXPECT_EQ(1, count_bits(1));
  EXPECT_EQ(1, count_bits(2));
  EXPECT_EQ(2, count_bits(3));
}

TEST(CountBits, Limits)
{
  EXPECT_EQ(32, count_bits(0xffffffff));
  EXPECT_EQ(64, count_bits(0xffffffffffffffffll));
}

TEST(CountBits, UnsignedLong)
{
  EXPECT_EQ(1, count_bits<unsigned long>(1));
  EXPECT_EQ(10, count_bits<unsigned long>(1023));
}

TEST(CountBits, Rgb30bpp)
{
  EXPECT_EQ(10, count_bits<unsigned long>(0x3ff));
  EXPECT_EQ(10, count_bits<unsigned long>(0xffc00));
  EXPECT_EQ(10, count_bits<unsigned long>(0x3ff00000));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
