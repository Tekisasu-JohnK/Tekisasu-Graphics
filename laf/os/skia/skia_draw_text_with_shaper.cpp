// LAF OS Library
// Copyright (C) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/draw_text.h"
#include "os/paint.h"
#include "os/skia/skia_helpers.h"
#include "os/skia/skia_surface.h"

#include "include/core/SkTextBlob.h"
#include "include/utils/SkTextUtils.h"
#include "modules/skshaper/include/SkShaper.h"

#include "include/core/SkCanvas.h"

#include <cfloat>

namespace os {

void draw_text_with_shaper(
  Surface* surface, Font* font,
  const std::string& text,
  const gfx::Point& pos,
  const Paint* paint,
  const TextAlign textAlign,
  DrawTextDelegate* delegate)
{
  // SkFont skFont(SkTypeface::MakeFromFile("/Library/Fonts/Arial Unicode.ttf"), SkIntToScalar(24));
  SkFont skFont(SkTypeface::MakeDefault(), SkIntToScalar(24));
  sk_sp<SkTextBlob> textBlob;
  auto shaper = SkShaper::Make();
  if (shaper) {
    SkTextBlobBuilderRunHandler builder(text.c_str(), { 0, 0 });
    shaper->shape(text.c_str(), text.size(), skFont, true, FLT_MAX, &builder);
    textBlob = builder.makeBlob();
  }
  else {
    textBlob = SkTextBlob::MakeFromText(text.c_str(), text.size(), skFont, SkTextEncoding::kUTF8);
  }

  if (textBlob)
    static_cast<SkiaSurface*>(surface)->canvas()
      .drawTextBlob(
        textBlob,
        SkIntToScalar(pos.x),
        SkIntToScalar(pos.y),
        (paint ? paint->skPaint(): SkPaint()));
}

} // namespace os
