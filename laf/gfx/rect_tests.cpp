// LAF Gfx Library
// Copyright (C) 2019-2022 Igara Studio S.A.
// Copyright (C) 2001-2013 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#include "gfx/border.h"
#include "gfx/rect.h"
#include "gfx/rect_io.h"
#include "gfx/size.h"

using namespace std;
using namespace gfx;

TEST(Rect, Ctor)
{
  EXPECT_EQ(Rect(0, 0, 0, 0), Rect());
  EXPECT_EQ(10, Rect(10, 20, 30, 40).x);
  EXPECT_EQ(20, Rect(10, 20, 30, 40).y);
  EXPECT_EQ(30, Rect(10, 20, 30, 40).w);
  EXPECT_EQ(40, Rect(10, 20, 30, 40).h);
}

TEST(Rect, Inflate)
{
  EXPECT_EQ(Rect(10, 20, 31, 42), Rect(10, 20, 30, 40).inflate(1, 2));
  EXPECT_EQ(Rect(10, 20, 31, 42), Rect(10, 20, 30, 40).inflate(Size(1, 2)));
}

TEST(Rect, Enlarge)
{
  EXPECT_EQ(Rect(9, 19, 32, 42), Rect(10, 20, 30, 40).enlarge(1));
  EXPECT_EQ(Rect(9, 18, 34, 46), Rect(10, 20, 30, 40).enlarge(Border(1, 2, 3, 4)));
  EXPECT_EQ(Rect(9, 18, 34, 46), Rect(10, 20, 30, 40) + Border(1, 2, 3, 4));
}

TEST(Rect, Shrink)
{
  EXPECT_EQ(Rect(11, 21, 28, 38), Rect(10, 20, 30, 40).shrink(1));
  EXPECT_EQ(Rect(11, 22, 26, 34), Rect(10, 20, 30, 40).shrink(Border(1, 2, 3, 4)));
  EXPECT_EQ(Rect(11, 22, 26, 34), Rect(10, 20, 30, 40) - Border(1, 2, 3, 4));
}

TEST(Rect, FitIn)
{
  EXPECT_EQ(Rect(10, 10, 8, 8), Rect(0, 0, 4, 4).fitIn(Rect(10, 10, 8, 8)));
  EXPECT_EQ(Rect(10, 10, 4, 4), Rect(0, 0, 8, 8).fitIn(Rect(10, 10, 4, 4)));

  EXPECT_EQ(Rect(10, 10, 8, 4), Rect(0, 0, 16, 8).fitIn(Rect(10, 10, 8, 4)));
  EXPECT_EQ(Rect(10, 12, 8, 4), Rect(0, 0, 16, 8).fitIn(Rect(10, 10, 8, 8)));
  EXPECT_EQ(Rect(13, 10, 2, 4), Rect(0, 0, 8, 16).fitIn(Rect(10, 10, 8, 4)));
}

TEST(Rect, Floor)
{
  EXPECT_EQ(gfx::Rect(0, 0, 1, 2), gfx::Rect(gfx::RectF(-0.25, -0.75, 1, 2)));
  EXPECT_EQ(gfx::Rect(-1, -1, 1, 2), gfx::RectF(-0.25, -0.75, 1, 2).floor());
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
