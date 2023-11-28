// LAF Base Library
// Copyright (c) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/ints.h"
#include "base/mask_shift.h"

using namespace base;

TEST(MaskShift, Uint8)
{
  EXPECT_EQ(0, mask_shift<uint8_t>(0b0000'0001));
  EXPECT_EQ(1, mask_shift<uint8_t>(0b0000'0010));
  EXPECT_EQ(2, mask_shift<uint8_t>(0b0000'0100));
  EXPECT_EQ(3, mask_shift<uint8_t>(0b0000'1000));
  EXPECT_EQ(4, mask_shift<uint8_t>(0b0001'0000));
  EXPECT_EQ(5, mask_shift<uint8_t>(0b0010'0000));
  EXPECT_EQ(6, mask_shift<uint8_t>(0b0100'0000));
  EXPECT_EQ(7, mask_shift<uint8_t>(0b1000'0000));
  EXPECT_EQ(8, mask_shift<uint8_t>(0b0000'0000));
}

TEST(MaskShift, Uint16)
{
  EXPECT_EQ(0, mask_shift<uint16_t>(0x000f));
  EXPECT_EQ(4, mask_shift<uint16_t>(0x00f0));
  EXPECT_EQ(8, mask_shift<uint16_t>(0x0f00));
  EXPECT_EQ(12, mask_shift<uint16_t>(0xf000));
  EXPECT_EQ(16, mask_shift<uint16_t>(0x0000));
}

TEST(MaskShift, Uint32)
{
  EXPECT_EQ(0, mask_shift<uint32_t>(0x0000000f));
  EXPECT_EQ(4, mask_shift<uint32_t>(0x000000f0));
  EXPECT_EQ(8, mask_shift<uint32_t>(0x00000f00));
  EXPECT_EQ(12, mask_shift<uint32_t>(0x0000f000));
  EXPECT_EQ(32, mask_shift<uint32_t>(0x00000000));

  EXPECT_EQ(0xff, (0xff00 >> mask_shift<uint32_t>(0xff00)));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
