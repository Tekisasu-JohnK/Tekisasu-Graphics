// LAF OS Library
// Copyright (C) 2018  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COLOR_SPACE_H_INCLUDED
#define OS_COLOR_SPACE_H_INCLUDED
#pragma once

#include "gfx/color_space.h"

#include <cstdint>
#include <memory>

namespace os {

  class ColorSpace {
  public:
    virtual ~ColorSpace() { }
    virtual const gfx::ColorSpacePtr& gfxColorSpace() const = 0;
  };

  typedef std::shared_ptr<ColorSpace> ColorSpacePtr;

  class ColorSpaceConversion {
  public:
    virtual ~ColorSpaceConversion() { }
    // Transform RGBA pixels between two color spaces.
    virtual bool convertRgba(uint32_t* dst, const uint32_t* src, int n) = 0;
    // Transform grayscale pixels (without alpha) between two color spaces.
    virtual bool convertGray(uint8_t* dst, const uint8_t* src, int n) = 0;
  };

} // namespace os

#endif
