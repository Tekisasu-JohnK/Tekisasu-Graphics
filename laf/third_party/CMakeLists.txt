# LAF
# Copyright (C) 2022  Igara Studio S.A.
# Copyright (C) 2016-2018  David Capello

# ----------------------------------------------------------------------
# googletest library

if(LAF_WITH_TESTS)
  set(gtest_force_shared_crt ON CACHE BOOL "")
  add_subdirectory(googletest)
endif()
