// LAF OS Library
// Copyright (C) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_HELPERS_INCLUDED
#define OS_SKIA_SKIA_HELPERS_INCLUDED
#pragma once

#include "gfx/color.h"
#include "gfx/rect.h"
#include "os/paint.h"
#include "os/sampling.h"

#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSamplingOptions.h"

#include <algorithm>

namespace os {

inline SkColor to_skia(gfx::Color c) {
  return SkColorSetARGB(gfx::geta(c), gfx::getr(c), gfx::getg(c), gfx::getb(c));
}

inline SkColor4f to_skia4f(gfx::Color c) {
  return SkColor4f::FromColor(to_skia(c));
}

inline SkIRect to_skia(const gfx::Rect& rc) {
  return SkIRect::MakeXYWH(rc.x, rc.y, rc.w, rc.h);
}

inline SkRect to_skia(const gfx::RectF& rc) {
  return SkRect::MakeXYWH(SkScalar(rc.x), SkScalar(rc.y), SkScalar(rc.w), SkScalar(rc.h));
}

inline SkRect to_skia_fix(const gfx::RectF& rc) {
  return SkRect::MakeXYWH(SkScalar(rc.x), SkScalar(rc.y),
                          SkScalar(std::max(0.0, rc.w-1)),
                          SkScalar(std::max(0.0, rc.h-1)));
}

inline void to_skia(const Paint& paint, SkPaint& skPaint) {
  static_assert((int)SkPaint::kFill_Style == (int)Paint::Fill &&
                (int)SkPaint::kStroke_Style == (int)Paint::Stroke &&
                (int)SkPaint::kStrokeAndFill_Style == (int)Paint::StrokeAndFill,
                "Paint styles don't match with Skia");

  static_assert((int)SkBlendMode::kClear == (int)BlendMode::Clear &&
                (int)SkBlendMode::kSrc == (int)BlendMode::Src &&
                (int)SkBlendMode::kDst == (int)BlendMode::Dst &&
                (int)SkBlendMode::kSrcOver == (int)BlendMode::SrcOver &&
                (int)SkBlendMode::kDstOver == (int)BlendMode::DstOver &&
                (int)SkBlendMode::kSrcIn == (int)BlendMode::SrcIn &&
                (int)SkBlendMode::kDstIn == (int)BlendMode::DstIn &&
                (int)SkBlendMode::kSrcOut == (int)BlendMode::SrcOut &&
                (int)SkBlendMode::kDstOut == (int)BlendMode::DstOut &&
                (int)SkBlendMode::kSrcATop == (int)BlendMode::SrcATop &&
                (int)SkBlendMode::kDstATop == (int)BlendMode::DstATop &&
                (int)SkBlendMode::kXor == (int)BlendMode::Xor &&
                (int)SkBlendMode::kPlus == (int)BlendMode::Plus &&
                (int)SkBlendMode::kModulate == (int)BlendMode::Modulate &&
                (int)SkBlendMode::kScreen == (int)BlendMode::Screen &&
                (int)SkBlendMode::kLastCoeffMode == (int)BlendMode::LastCoeffMode &&
                (int)SkBlendMode::kOverlay == (int)BlendMode::Overlay &&
                (int)SkBlendMode::kDarken == (int)BlendMode::Darken &&
                (int)SkBlendMode::kLighten == (int)BlendMode::Lighten &&
                (int)SkBlendMode::kColorDodge == (int)BlendMode::ColorDodge &&
                (int)SkBlendMode::kColorBurn == (int)BlendMode::ColorBurn &&
                (int)SkBlendMode::kHardLight == (int)BlendMode::HardLight &&
                (int)SkBlendMode::kSoftLight == (int)BlendMode::SoftLight &&
                (int)SkBlendMode::kDifference == (int)BlendMode::Difference &&
                (int)SkBlendMode::kExclusion == (int)BlendMode::Exclusion &&
                (int)SkBlendMode::kMultiply == (int)BlendMode::Multiply &&
                (int)SkBlendMode::kLastSeparableMode == (int)BlendMode::LastSeparableMode &&
                (int)SkBlendMode::kHue == (int)BlendMode::Hue &&
                (int)SkBlendMode::kSaturation == (int)BlendMode::Saturation &&
                (int)SkBlendMode::kColor == (int)BlendMode::Color &&
                (int)SkBlendMode::kLuminosity == (int)BlendMode::Luminosity &&
                (int)SkBlendMode::kLastMode == (int)BlendMode::LastMode,
                "Blend modes don't match with Skia");

  skPaint.setColor(to_skia(paint.color()));
  skPaint.setStyle((SkPaint::Style)paint.style());
  skPaint.setAntiAlias(paint.antialias());
  skPaint.setStrokeWidth(paint.strokeWidth());
  skPaint.setBlendMode((SkBlendMode)paint.blendMode());
}

inline void to_skia(const Sampling& sampling, SkSamplingOptions& skSampling) {
  static_assert((int)SkFilterMode::kNearest == (int)Sampling::Filter::Nearest &&
                (int)SkFilterMode::kLinear == (int)Sampling::Filter::Linear,
                "Sampling filter modes don't match with Skia");

  static_assert((int)SkMipmapMode::kNone == (int)Sampling::Mipmap::None &&
                (int)SkMipmapMode::kNearest == (int)Sampling::Mipmap::Nearest &&
                (int)SkMipmapMode::kLinear == (int)Sampling::Mipmap::Linear,
                "Sampling mipmap modes don't match with Skia");

  if (sampling.useCubic) {
    skSampling = SkSamplingOptions({ sampling.cubic.B,
                                     sampling.cubic.C });
  }
  else {
    skSampling = SkSamplingOptions((SkFilterMode)sampling.filter,
                                   (SkMipmapMode)sampling.mipmap);
  }
}

} // namespace os

#endif
