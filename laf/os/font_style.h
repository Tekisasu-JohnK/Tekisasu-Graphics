// LAF OS Library
// Copyright (C) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_FONT_STYLE_H_INCLUDED
#define OS_FONT_STYLE_H_INCLUDED
#pragma once

#include "base/ints.h"

namespace os {

  class FontStyle {
  public:
    enum class Weight {
      Invisible   =    0,
      Thin        =  100,
      ExtraLight  =  200,
      Light       =  300,
      Normal      =  400,
      Medium      =  500,
      SemiBold    =  600,
      Bold        =  700,
      ExtraBold   =  800,
      Black       =  900,
      ExtraBlack  = 1000,
    };

    enum class Width {
      UltraCondensed   = 1,
      ExtraCondensed   = 2,
      Condensed        = 3,
      SemiCondensed    = 4,
      Normal           = 5,
      SemiExpanded     = 6,
      Expanded         = 7,
      ExtraExpanded    = 8,
      UltraExpanded    = 9,
    };

    enum class Slant {
      Upright,
      Italic,
      Oblique,
    };

    constexpr FontStyle(const Weight weight,
                        const Width width,
                        const Slant slant)
      : m_value(int(weight) |
                (int(width) << 16) |
                (int(slant) << 24)) { }

    constexpr FontStyle() : FontStyle(Weight::Normal,
                                      Width::Normal,
                                      Slant::Upright) { }

    bool operator==(const FontStyle& other) const {
      return m_value == other.m_value;
    }

    Weight weight() const { return Weight(m_value & 0xFFFF); }
    Width width() const { return Width((m_value >> 16) & 0xFF); }
    Slant slant() const { return Slant((m_value >> 24) & 0xFF); }

    static constexpr FontStyle Normal() {
      return FontStyle(Weight::Normal, Width::Normal, Slant::Upright);
    }

    static constexpr FontStyle Bold() {
      return FontStyle(Weight::Bold, Width::Normal, Slant::Upright);
    }

    static constexpr FontStyle Italic() {
      return FontStyle(Weight::Normal, Width::Normal, Slant::Italic);
    }

    static constexpr FontStyle BoldItalic() {
      return FontStyle(Weight::Bold, Width::Normal, Slant::Italic);
    }

  private:
    uint32_t m_value;
  };

} // namespace os

#endif
