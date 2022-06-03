// LAF OS Library
// Copyright (c) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_PAINT_H_INCLUDED
#define OS_PAINT_H_INCLUDED
#pragma once

#include "gfx/color.h"

namespace os {

  class Paint {
  public:
    enum Flags {
      kNone = 0,
      kNineWithoutCenter = 1,
      kCenterAlign = 2,
      kEndAlign = 4,
      kAntialias = 8,
    };

    enum Style {
      Fill,
      Stroke,
      StrokeAndFill,
    };

    Flags flags() const { return m_flags; }
    void flags(const Flags flags) { m_flags = flags; }
    void setFlags(const Flags flags) {
      m_flags = Flags(int(m_flags) | int(flags));
    }
    void resetFlags(const Flags flags) {
      m_flags = Flags(int(m_flags) & ~int(flags));
    }
    bool hasFlags(const Flags flags) const {
      return (int(m_flags) & int(flags)) == int(flags);
    }

    bool antialias() const { return hasFlags(kAntialias); }
    void antialias(const bool state) {
      if (state)
        return setFlags(kAntialias);
      else
        return resetFlags(kAntialias);
    }

    Style style() const { return m_style; }
    void style(const Style style) { m_style = style; }

    gfx::Color color() const { return m_color; }
    void color(const gfx::Color color) { m_color = color; }

  private:
    Flags m_flags = kNone;
    Style m_style = Fill;
    gfx::Color m_color = gfx::ColorNone;
  };

} // namespace os

#endif
