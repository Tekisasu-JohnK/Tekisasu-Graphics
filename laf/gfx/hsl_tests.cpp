// LAF Gfx Library
// Copyright (C) 2020 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#include "gfx/hsl.h"
#include "gfx/rgb.h"

using namespace gfx;
using namespace std;

namespace gfx {

  ostream& operator<<(ostream& os, const Hsl& hsl)
  {
    return os << "("
              << hsl.hueInt() << ", "
              << hsl.saturationInt() << ", "
              << hsl.lightnessInt() << "); real: ("
              << hsl.hue() << ", "
              << hsl.saturation() << ", "
              << hsl.lightness() << ")";
  }

}

TEST(Hsl, Ctor)
{
  EXPECT_EQ(35.0, Hsl(35.0, 0.50, 0.75).hue());
  EXPECT_EQ(0.50, Hsl(35.0, 0.50, 0.75).saturation());
  EXPECT_EQ(0.75, Hsl(35.0, 0.50, 0.75).lightness());
  EXPECT_EQ(35, Hsl(35.0, 0.50, 0.75).hueInt());
  EXPECT_EQ(50, Hsl(35.0, 0.50, 0.75).saturationInt());
  EXPECT_EQ(75, Hsl(35.0, 0.50, 0.75).lightnessInt());
  EXPECT_EQ(Hsl(0, 0, 0), Hsl());
}

TEST(Hsl, FromRgb)
{
  EXPECT_EQ(Hsl(  0.0, 0.00, 0.00), Hsl(Rgb(  0,   0,   0)));
  EXPECT_EQ(Hsl(  0.0, 1.00, 0.01), Hsl(Rgb(  3,   0,   0)));
  EXPECT_EQ(Hsl(  0.0, 1.00, 0.99), Hsl(Rgb(255, 250, 250)));
  EXPECT_EQ(Hsl(  0.0, 0.66, 0.50), Hsl(Rgb(212,  43,  43)));
  EXPECT_EQ(Hsl( 60.0, 1.00, 0.75), Hsl(Rgb(255, 255, 128)));
  EXPECT_EQ(Hsl(120.0, 1.00, 0.50), Hsl(Rgb(  0, 255,   0)));
  EXPECT_EQ(Hsl(  0.0, 0.00, 1.00), Hsl(Rgb(255, 255, 255)));
  EXPECT_EQ(Hsl(180.0, 0.50, 0.50), Hsl(Rgb( 64, 191, 191)));
  EXPECT_EQ(Hsl(240.0, 0.50, 0.75), Hsl(Rgb(159, 159, 223)));
  EXPECT_EQ(Hsl(240.0, 1.00, 0.25), Hsl(Rgb(  0,   0, 128)));
  EXPECT_EQ(Hsl(300.0, 0.66, 0.75), Hsl(Rgb(233, 149, 233)));
  EXPECT_EQ(Hsl(  0.0, 1.00, 0.66), Hsl(Rgb(255,  82,  82)));
  EXPECT_EQ(Hsl(  0.0, 1.00, 0.67), Hsl(Rgb(255,  87,  87)));
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
