// LAF Gfx Library
// Copyright (C) 2019-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_REGION_H_INCLUDED
#define GFX_REGION_H_INCLUDED
#pragma once

#include "gfx/rect.h"
#include <vector>
#include <iterator>

#if LAF_SKIA
  // There is a header file on Skia (SkTFitsIn.h) that uses
  // std::numeric_limits<>::max() and fails if we don't undef the
  // max() macro.
  #if LAF_WINDOWS
    #undef max
  #endif

  #include "gfx/region_skia.h"
#elif LAF_PIXMAN
  #include "gfx/region_pixman.h"
#elif LAF_WINDOWS
  #include "gfx/region_win.h"
#endif

#endif
