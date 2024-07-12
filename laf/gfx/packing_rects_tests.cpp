// LAF Gfx Library
// Copyright (C) 2019-2021  Igara Studio S.A.
// Copyright (C) 2001-2014 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#if LAF_WITH_REGION

#include "gfx/packing_rects.h"
#include "gfx/rect_io.h"
#include "gfx/size.h"

using namespace gfx;

TEST(PackingRects, Simple)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(256, 128));
  EXPECT_FALSE(pr.pack(Size(256, 120), token));
  EXPECT_TRUE(pr.pack(Size(256, 128), token));

  EXPECT_EQ(Rect(0, 0, 256, 128), pr[0]);
  EXPECT_EQ(Rect(0, 0, 256, 128), pr.bounds());
}

TEST(PackingRects, SimpleTwoRects)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(256, 128));
  pr.add(Size(256, 120));
  EXPECT_TRUE(pr.pack(Size(256, 256), token));

  EXPECT_EQ(Rect(0, 0, 256, 256), pr.bounds());
  EXPECT_EQ(Rect(0, 0, 256, 128), pr[0]);
  EXPECT_EQ(Rect(0, 128, 256, 120), pr[1]);
}

TEST(PackingRects, BestFit)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(10, 12));
  pr.bestFit(token);
  EXPECT_EQ(Rect(0, 0, 10, 12), pr.bounds());
}

TEST(PackingRects, BestFitTwoRects)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(256, 128));
  pr.add(Size(256, 127));
  pr.bestFit(token);

  EXPECT_EQ(Rect(0, 0, 512, 128), pr.bounds());
  EXPECT_EQ(Rect(0, 0, 256, 128), pr[0]);
  EXPECT_EQ(Rect(256, 0, 256, 127), pr[1]);
}

TEST(PackingRects, BestFit6Frames100x100)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(100, 100));
  pr.add(Size(100, 100));
  pr.add(Size(100, 100));
  pr.add(Size(100, 100));
  pr.add(Size(100, 100));
  pr.add(Size(100, 100));
  pr.bestFit(token);

  EXPECT_EQ(Rect(0, 0, 300, 200), pr.bounds());
  EXPECT_EQ(Rect(0, 0, 100, 100), pr[0]);
  EXPECT_EQ(Rect(100, 0, 100, 100), pr[1]);
  EXPECT_EQ(Rect(200, 0, 100, 100), pr[2]);
  EXPECT_EQ(Rect(0, 100, 100, 100), pr[3]);
  EXPECT_EQ(Rect(100, 100, 100, 100), pr[4]);
  EXPECT_EQ(Rect(200, 100, 100, 100), pr[5]);
}

TEST(PackingRects, SmallerRectanglesAtTheEnd)
{
  base::task_token token;
  PackingRects pr;
  pr.add(Size(10, 10));
  pr.add(Size(20, 20));
  pr.add(Size(30, 30));
  pr.bestFit(token);

  EXPECT_EQ(Rect(0, 0, 60, 30), pr.bounds());
  EXPECT_EQ(Rect(50, 0, 10, 10), pr[0]);
  EXPECT_EQ(Rect(30, 0, 20, 20), pr[1]);
  EXPECT_EQ(Rect(0, 0, 30, 30), pr[2]);
}

TEST(PackingRects, BorderAndShapePadding)
{
  base::task_token token;

  PackingRects pr(10, 3);
  pr.add(Size(200, 100));
  pr.add(Size(200, 100));
  pr.add(Size(200, 100));

  EXPECT_FALSE(pr.pack(Size(220, 325), token));
  EXPECT_FALSE(pr.pack(Size(219, 326), token));
  EXPECT_TRUE(pr.pack(Size(220, 326), token));

  EXPECT_EQ(Rect(10, 10, 200, 100), pr[0]);
  EXPECT_EQ(Rect(10, 113, 200, 100), pr[1]);
  EXPECT_EQ(Rect(10, 216, 200, 100), pr[2]);
}

#endif  // LAF_WITH_REGION

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
