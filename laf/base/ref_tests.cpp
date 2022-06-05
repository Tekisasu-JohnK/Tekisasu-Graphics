// LAF Base Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <gtest/gtest.h>

#include "base/ref.h"

using namespace base;

int ctors = 0;
int dtors = 0;

class A : public RefCountT<A> {
public:
  A(int v) : v(v) { ++ctors; }
  ~A() { ++dtors; }
  int v = 0;
};

TEST(Ref, Stack)
{
  ctors = dtors = 0;
  {
    A a(2);
    EXPECT_EQ(2, a.v);
  }
  EXPECT_EQ(1, ctors);
  EXPECT_EQ(1, dtors);
}

TEST(Ref, AssignOperator)
{
  ctors = dtors = 0;
  {
    auto a = make_ref<A>(1);
    auto b = make_ref<A>(2);
    auto c = a;
    EXPECT_EQ(1, c->v);
    c = b;
    EXPECT_EQ(2, c->v);
    a.reset();
  }
  EXPECT_EQ(2, ctors);
  EXPECT_EQ(2, dtors);
}

TEST(Ref, Operators)
{
  Ref<A> a = make_ref<A>(1);
  Ref<A> b = make_ref<A>(2);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(a != b);
  EXPECT_TRUE(a != nullptr);
  EXPECT_TRUE(nullptr != a);
  EXPECT_FALSE(a == nullptr);
  EXPECT_FALSE(nullptr == a);

  Ref<A> c;
  EXPECT_TRUE(c == nullptr);
  EXPECT_TRUE(nullptr == c);
  EXPECT_FALSE(c != nullptr);
  EXPECT_FALSE(nullptr != c);
}

TEST(RefCount, Swap)
{
  ctors = dtors = 0;
  {
    Ref<A> a = make_ref<A>(1);
    Ref<A> b = make_ref<A>(2);
    std::swap(a, b);
  }
  EXPECT_EQ(2, ctors);
  EXPECT_EQ(2, dtors);
}

int b_dtors = 0;

class AVirt : public RefCount {
public:
  AVirt(int v) : v(v) { ++ctors; }
  ~AVirt() { ++dtors; }
  int v = 0;
};

class BVirt : public AVirt {
public:
  BVirt(int v) : AVirt(v) { }
  ~BVirt() { ++b_dtors; }
};

TEST(RefCount, Virtual)
{
  ctors = dtors = b_dtors = 0;
  {
    Ref<AVirt> b = make_ref<BVirt>(1);
  }
  EXPECT_EQ(1, ctors);
  EXPECT_EQ(1, dtors);
  EXPECT_EQ(1, b_dtors);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
