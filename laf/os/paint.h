// LAF OS Library
// Copyright (c) 2019-2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_PAINT_H_INCLUDED
#define OS_PAINT_H_INCLUDED
#pragma once

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

  class PaintBase {
  public:
    // Same as SkPaint::Style
    enum Style {
      Fill,
      Stroke,
      StrokeAndFill,
    };

    // Same as SkCanvas::SrcRectConstraint
    enum SrcEdges {
      Strict, // Sample only inside bounds, is slower (don't use mipmaps)
      Fast,   // Sample outside bounds, is faster (use mipmaps)
    };

    SrcEdges srcEdges() const { return m_srcEdges; }
    void srcEdges(const SrcEdges srcEdges) { m_srcEdges = srcEdges; }

  private:
    SrcEdges m_srcEdges = SrcEdges::Strict;
  };

};

#if LAF_SKIA
  #include "os/skia/paint.h"
#else
  #include "os/none/paint.h"
#endif

#endif
