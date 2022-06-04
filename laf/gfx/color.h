// LAF Gfx Library
// Copyright (C) 2021 Igara Studio S.A.
// Copyright (C) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_COLOR_H_INCLUDED
#define GFX_COLOR_H_INCLUDED
#pragma once

#include "base/ints.h"

namespace gfx {

  typedef uint32_t Color;
  typedef uint8_t ColorComponent;

  static const uint32_t ColorRShift = 0;
  static const uint32_t ColorGShift = 8;
  static const uint32_t ColorBShift = 16;
  static const uint32_t ColorAShift = 24;

  static const uint32_t ColorRMask = 0x000000ff;
  static const uint32_t ColorGMask = 0x0000ff00;
  static const uint32_t ColorBMask = 0x00ff0000;
  static const uint32_t ColorAMask = 0xff000000;
  static const uint32_t ColorRGBMask = 0x00ffffff;

  static const Color ColorNone = Color(0);

  inline Color rgba(ColorComponent r, ColorComponent g, ColorComponent b, ColorComponent a = 255) {
    return Color((r << ColorRShift) |
                 (g << ColorGShift) |
                 (b << ColorBShift) |
                 (a << ColorAShift));
  }

  inline ColorComponent getr(Color c) { return (c >> ColorRShift) & 0xff; }
  inline ColorComponent getg(Color c) { return (c >> ColorGShift) & 0xff; }
  inline ColorComponent getb(Color c) { return (c >> ColorBShift) & 0xff; }
  inline ColorComponent geta(Color c) { return (c >> ColorAShift) & 0xff; }

  inline Color seta(Color c, ColorComponent a) {
    return (c & ColorRGBMask) | (a << ColorAShift);
  }

  inline bool is_transparent(Color c) { return geta(c) == 0; }

} // namespace gfx

#endif  // GFX_COLOR_H_INCLUDED
