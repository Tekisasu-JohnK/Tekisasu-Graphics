// LAF OS Library
// Copyright (c) 2019-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_PAINT_H_INCLUDED
#define OS_PAINT_H_INCLUDED
#pragma once

#include "gfx/color.h"

namespace os {

  // Same values as SkBlendMode
  enum class BlendMode {
    Clear,
    Src,
    Dst,
    SrcOver,
    DstOver,
    SrcIn,
    DstIn,
    SrcOut,
    DstOut,
    SrcATop,
    DstATop,
    Xor,
    Plus,
    Modulate,
    Screen,
    LastCoeffMode = Screen,
    Overlay,
    Darken,
    Lighten,
    ColorDodge,
    ColorBurn,
    HardLight,
    SoftLight,
    Difference,
    Exclusion,
    Multiply,
    LastSeparableMode = Multiply,
    Hue,
    Saturation,
    Color,
    Luminosity,
    LastMode = Luminosity,
  };

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

    float strokeWidth() const { return m_strokeWidth; }
    void strokeWidth(const float strokeWidth) { m_strokeWidth = strokeWidth; }

    BlendMode blendMode() const { return m_blendMode; }
    void blendMode(const BlendMode blendMode) { m_blendMode = blendMode; }

  private:
    Flags m_flags = kNone;
    Style m_style = Fill;
    // Opaque black must be the default (to match SkPaint default)
    gfx::Color m_color = gfx::rgba(0, 0, 0, 255);
    float m_strokeWidth = 1.0f;
    BlendMode m_blendMode = BlendMode::SrcOver;
  };

} // namespace os

#endif
