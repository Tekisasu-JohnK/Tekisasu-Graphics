// LAF OS Library
// Copyright (c) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COLOR_SPACE_H_INCLUDED
#define OS_COLOR_SPACE_H_INCLUDED
#pragma once

#include "gfx/color_space.h"
#include "os/ref.h"

#include <cstdint>

namespace os {

  class ColorSpace;
  using ColorSpaceRef = Ref<ColorSpace>;

  class ColorSpace : public RefCount {
  public:
    virtual ~ColorSpace() { }
    virtual const gfx::ColorSpaceRef& gfxColorSpace() const = 0;
    virtual const bool isSRGB() const = 0;
  };

  class ColorSpaceConversion : public RefCount {
  public:
    virtual ~ColorSpaceConversion() { }
    // Transform RGBA pixels between two color spaces.
    virtual bool convertRgba(uint32_t* dst, const uint32_t* src, int n) = 0;
    // Transform grayscale pixels (without alpha) between two color spaces.
    virtual bool convertGray(uint8_t* dst, const uint8_t* src, int n) = 0;
  };

} // namespace os

#endif
