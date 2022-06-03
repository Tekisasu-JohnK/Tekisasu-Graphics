// LAF OS Library
// Copyright (C) 2019-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_HELPERS_INCLUDED
#define OS_SKIA_SKIA_HELPERS_INCLUDED
#pragma once

#include "gfx/color.h"
#include "gfx/rect.h"
#include "os/paint.h"

#include "SkColor.h"
#include "SkPaint.h"
#include "SkRect.h"

#include <algorithm>

namespace os {

inline SkColor to_skia(gfx::Color c) {
  return SkColorSetARGB(gfx::geta(c), gfx::getr(c), gfx::getg(c), gfx::getb(c));
}

inline SkIRect to_skia(const gfx::Rect& rc) {
  return SkIRect::MakeXYWH(rc.x, rc.y, rc.w, rc.h);
}

inline SkRect to_skia(const gfx::RectF& rc) {
  return SkRect::MakeXYWH(rc.x, rc.y, rc.w, rc.h);
}

inline SkRect to_skia_fix(const gfx::RectF& rc) {
  return SkRect::MakeXYWH(rc.x, rc.y,
                          std::max(0.0, rc.w-1),
                          std::max(0.0, rc.h-1));
}

inline void to_skia(const Paint& paint, SkPaint& skPaint) {
  static_assert((int)SkPaint::kFill_Style == (int)Paint::Fill &&
                (int)SkPaint::kStroke_Style == (int)Paint::Stroke &&
                (int)SkPaint::kStrokeAndFill_Style == (int)Paint::StrokeAndFill,
                "Paint styles don't match with Skia");

  skPaint.setColor(to_skia(paint.color()));
  skPaint.setStyle((SkPaint::Style)paint.style());
  skPaint.setAntiAlias(paint.antialias());
}

} // namespace os

#endif
