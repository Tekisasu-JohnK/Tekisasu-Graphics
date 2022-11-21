// Aseprite Document Library
// Copyright (c) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#include "doc/color_scales.h"

#include <cmath>
#include <cstdlib>

using namespace doc;

TEST(Scale, MatchValues)
{
  for (int x=1; x<=8; ++x) {
    switch (x) {
      case 3:
        for (int v=0; v<8; ++v)
          EXPECT_EQ(scale_3bits_to_8bits(v), scale_xxbits_to_8bits(3, v));
        break;
      case 5:
        for (int v=0; v<32; ++v)
          EXPECT_EQ(scale_5bits_to_8bits(v), scale_xxbits_to_8bits(5, v));
        break;
      case 6:
        for (int v=0; v<64; ++v)
          EXPECT_EQ(scale_6bits_to_8bits(v), scale_xxbits_to_8bits(6, v));
        break;
    }
    for (int v=0; v<(1<<x); ++v)
      EXPECT_LE(std::abs((255 * v / ((1<<x)-1)) - scale_xxbits_to_8bits(x, v)), 1);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
