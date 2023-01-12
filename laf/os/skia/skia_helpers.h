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
