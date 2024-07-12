// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#include "os/system.h"

TEST(System, CtorDtor)
{
  auto system = os::make_system();
}

int app_main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
