// LAF OS Library
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtest/gtest.h>

#include "os/scoped_handle.h"
#include "os/system.h"

using namespace os;

TEST(System, CtorDtor)
{
  SystemHandle sys(create_system());
}

int app_main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
