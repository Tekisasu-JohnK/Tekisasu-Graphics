// LAF Base Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/enum_flags.h"

#include "base/ints.h"

enum class U8 : uint8_t { A=1, B=2, C=4 };
enum class U32 : uint32_t { A=1, B=256 };

LAF_ENUM_FLAGS(U8);
LAF_ENUM_FLAGS(U32);

TEST(EnumFlags, Uint8)
{
  U8 a = { };
  EXPECT_EQ(U8(0), a);

  a |= U8::A; EXPECT_EQ(U8::A, a);
  a |= U8::B; EXPECT_EQ(U8::A | U8::B, a);

  EXPECT_EQ(U8::B, a & U8::B);
  EXPECT_EQ(U8(0), a & U8::C);
  EXPECT_EQ(U8::A | U8::B, a & (U8::A | U8::B));

  a &= U8::B;
  EXPECT_EQ(U8::B, a);

  a ^= U8::C;
  EXPECT_EQ(U8::B | U8::C, a);
}

TEST(EnumFlags, Conversion)
{
  U32 a = U32::A | U32::B;
  EXPECT_EQ(U32::A | U32::B, a);

  EXPECT_EQ(uint32_t(U8::A), uint32_t(U32::A));

  U8 b = U8(a);
  EXPECT_EQ(U8::A, b);          // U32::B is lost in uint8_t
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
