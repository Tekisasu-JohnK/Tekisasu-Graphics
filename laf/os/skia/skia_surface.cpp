// LAF OS Library
// Copyright (c) 2018-2020  Igara Studio S.A.
// Copyright (c) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/skia/skia_surface.h"

#include "base/file_handle.h"
#include "gfx/path.h"
#include "os/skia/skia_helpers.h"

#include "SkCodec.h"
#include "SkPixelRef.h"
#include "SkStream.h"

#include <memory>

namespace os {

void SkiaSurface::drawLine(const float x0, const float y0,
                           const float x1, const float y1,
                           const Paint& paint)
{
  to_skia(paint, m_paint);
  m_canvas->drawLine(x0, y0, x1, y1, m_paint);
}

void SkiaSurface::drawRect(const gfx::RectF& rc,
                           const Paint& paint)
{
  if (rc.isEmpty())
    return;

  to_skia(paint, m_paint);
  if (paint.style() == Paint::Style::Stroke) {
    m_canvas->drawRect(to_skia_fix(rc), m_paint);
  }
  else {
    m_canvas->drawRect(to_skia(rc), m_paint);
  }
}

void SkiaSurface::drawCircle(const float cx, const float cy,
                             const float radius,
                             const Paint& paint)
{
  to_skia(paint, m_paint);
  m_canvas->drawCircle(cx, cy, radius, m_paint);
}

void SkiaSurface::drawPath(const gfx::Path& path,
                           const Paint& paint)
{
  to_skia(paint, m_paint);
  m_canvas->drawPath(path.skPath(), m_paint);
}

// static
Surface* SkiaSurface::loadSurface(const char* filename)
{
  FILE* f = base::open_file_raw(filename, "rb");
  if (!f)
    return nullptr;

  std::unique_ptr<SkCodec> codec(
    SkCodec::MakeFromStream(
      std::unique_ptr<SkFILEStream>(new SkFILEStream(f))));
  if (!codec)
    return nullptr;

  SkImageInfo info = codec->getInfo()
    .makeColorType(kN32_SkColorType)
    .makeAlphaType(kPremul_SkAlphaType);
  SkBitmap bm;
  if (!bm.tryAllocPixels(info))
    return nullptr;

  const SkCodec::Result r = codec->getPixels(info, bm.getPixels(), bm.rowBytes());
  if (r != SkCodec::kSuccess)
    return nullptr;

  SkiaSurface* sur = new SkiaSurface();
  sur->swapBitmap(bm);
  return sur;
}

} // namespace os
