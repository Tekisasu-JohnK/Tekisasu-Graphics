// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/dll.h"

#if LAF_WINDOWS
  #include "base/dll_win32.h"
#else
  #include "base/dll_unix.h"
#endif
