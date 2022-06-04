// LAF OS Library
// Copyright (c) 2018-2022  Igara Studio S.A.
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

#include "include/codec/SkCodec.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkPixelRef.h"
#include "include/core/SkStream.h"
#include "include/private/SkColorData.h"

#include <memory>

namespace os {

SkiaSurface::SkiaSurface()
  : m_surface(nullptr)
  , m_colorSpace(nullptr)
  , m_canvas(nullptr)
  , m_lock(0)
{
}

SkiaSurface::SkiaSurface(const sk_sp<SkSurface>& surface)
  : m_surface(surface)
  , m_colorSpace(nullptr)
  , m_canvas(nullptr)
  , m_lock(0)
{
  ASSERT(m_surface);
  if (m_surface)
    m_canvas = m_surface->getCanvas();
}

SkiaSurface::~SkiaSurface()
{
  ASSERT(m_lock == 0);
  destroy();
}

void SkiaSurface::create(int width, int height, const os::ColorSpaceRef& cs)
{
  destroy();

  ASSERT(!m_surface)
    ASSERT(width > 0);
  ASSERT(height > 0);

  m_colorSpace = cs;

  SkBitmap bmp;
  if (!bmp.tryAllocPixels(
        SkImageInfo::MakeN32(width, height, kOpaque_SkAlphaType, skColorSpace())))
    throw base::Exception("Cannot create Skia surface");

  bmp.eraseColor(SK_ColorTRANSPARENT);
  swapBitmap(bmp);
}

void SkiaSurface::createRgba(int width, int height, const os::ColorSpaceRef& cs)
{
  destroy();

  ASSERT(!m_surface);
  ASSERT(width > 0);
  ASSERT(height > 0);

  m_colorSpace = cs;

  SkBitmap bmp;
  if (!bmp.tryAllocPixels(
        SkImageInfo::MakeN32Premul(width, height, skColorSpace())))
    throw base::Exception("Cannot create Skia surface");

  bmp.eraseColor(SK_ColorTRANSPARENT);
  swapBitmap(bmp);
}

void SkiaSurface::destroy()
{
  if (!m_surface) {
    delete m_canvas;
    m_canvas = nullptr;
  }
}

void SkiaSurface::flush() const
{
  if (m_canvas)
    m_canvas->flush();
}

void SkiaSurface::flushAndSubmit() const
{
  if (m_surface)
    m_surface->flushAndSubmit();
}

int SkiaSurface::width() const
{
  if (m_surface)
    return m_surface->width();
  else
    return m_bitmap.width();
}

int SkiaSurface::height() const
{
  if (m_surface)
    return m_surface->height();
  else
    return m_bitmap.height();
}

const ColorSpaceRef& SkiaSurface::colorSpace() const
{
  return m_colorSpace;
}

bool SkiaSurface::isDirectToScreen() const
{
  return false;
}

void SkiaSurface::setImmutable()
{
  if (!m_bitmap.isNull())
    m_bitmap.setImmutable();
}

int SkiaSurface::getSaveCount() const
{
  return m_canvas->getSaveCount();
}

gfx::Rect SkiaSurface::getClipBounds() const
{
  SkIRect rc;
  if (m_canvas->getDeviceClipBounds(&rc))
    return gfx::Rect(rc.x(), rc.y(), rc.width(), rc.height());
  else
    return gfx::Rect();
}

void SkiaSurface::saveClip()
{
  m_canvas->save();
}

void SkiaSurface::restoreClip()
{
  m_canvas->restore();
}

bool SkiaSurface::clipRect(const gfx::Rect& rc)
{
  m_canvas->clipRect(SkRect::Make(to_skia(rc)));
  return !m_canvas->isClipEmpty();
}

void SkiaSurface::save()
{
  m_canvas->save();
}

void SkiaSurface::concat(const gfx::Matrix& matrix)
{
  m_canvas->concat(matrix.skMatrix());
}

void SkiaSurface::setMatrix(const gfx::Matrix& matrix)
{
  m_canvas->setMatrix(matrix.skMatrix());
}

void SkiaSurface::resetMatrix()
{
  m_canvas->resetMatrix();
}

void SkiaSurface::restore()
{
  m_canvas->restore();
}

gfx::Matrix SkiaSurface::matrix() const
{
  return m_canvas->getTotalMatrix();
}

void SkiaSurface::setDrawMode(DrawMode mode, int param,
                              const gfx::Color a,
                              const gfx::Color b)
{
  switch (mode) {
    case DrawMode::Solid:
      m_paint.setBlendMode(SkBlendMode::kSrcOver);
      m_paint.setShader(nullptr);
      break;
    case DrawMode::Xor:
      m_paint.setBlendMode(SkBlendMode::kXor);
      m_paint.setShader(nullptr);
      break;
    case DrawMode::Checked: {
      m_paint.setBlendMode(SkBlendMode::kSrcOver);
      {
        SkBitmap bitmap;
        if (!bitmap.tryAllocPixels(
              SkImageInfo::MakeN32(8, 8, kOpaque_SkAlphaType, skColorSpace()))) {
          throw base::Exception("Cannot create temporary Skia surface");
        }

        {
          SkPMColor A = SkPreMultiplyARGB(gfx::geta(a), gfx::getr(a), gfx::getg(a), gfx::getb(a));
          SkPMColor B = SkPreMultiplyARGB(gfx::geta(b), gfx::getr(b), gfx::getg(b), gfx::getb(b));
          int offset = 7 - (param & 7);
          for (int y=0; y<8; y++)
            for (int x=0; x<8; x++)
              *bitmap.getAddr32(x, y) = (((x+y+offset)&7) < 4 ? B: A);
        }

        sk_sp<SkShader> shader(
          bitmap.makeShader(SkTileMode::kRepeat,
                            SkTileMode::kRepeat,
                            SkSamplingOptions()));
        m_paint.setShader(shader);
      }
      break;
    }
  }
}

void SkiaSurface::lock()
{
  ASSERT(m_lock >= 0);
  if (m_lock++ == 0) {
    // TODO add mutex!
    // m_bitmap is always locked
  }
}

void SkiaSurface::unlock()
{
  ASSERT(m_lock > 0);
  if (--m_lock == 0) {
    // m_bitmap is always locked
  }
}

void SkiaSurface::applyScale(int scaleFactor)
{
  ASSERT(!m_surface);

  SkBitmap result;
  if (!result.tryAllocPixels(
        m_bitmap.info().makeWH(
          width()*scaleFactor,
          height()*scaleFactor)))
    throw base::Exception("Cannot create temporary Skia surface to change scale");

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrc);

  SkCanvas canvas(result);
  SkRect srcRect = SkRect::Make(SkIRect::MakeXYWH(0, 0, width(), height()));
  SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(0, 0, result.width(), result.height()));
  canvas.drawImageRect(SkImage::MakeFromRaster(m_bitmap.pixmap(), nullptr, nullptr),
                       srcRect, dstRect, SkSamplingOptions(),
                       &paint, SkCanvas::kStrict_SrcRectConstraint);

  swapBitmap(result);
}

void* SkiaSurface::nativeHandle()
{
  return (void*)this;
}

void SkiaSurface::clear()
{
  m_canvas->clear(0);
}

uint8_t* SkiaSurface::getData(int x, int y) const
{
  if (m_bitmap.isNull())
    return nullptr;
  else
    return (uint8_t*)m_bitmap.getAddr32(x, y);
}

void SkiaSurface::getFormat(SurfaceFormatData* formatData) const
{
  formatData->format = kRgbaSurfaceFormat;
  formatData->bitsPerPixel = 8*m_bitmap.bytesPerPixel();

  switch (m_bitmap.colorType()) {
    case kRGB_565_SkColorType:
      formatData->redShift   = SK_R16_SHIFT;
      formatData->greenShift = SK_G16_SHIFT;
      formatData->blueShift  = SK_B16_SHIFT;
      formatData->alphaShift = 0;
      formatData->redMask    = SK_R16_MASK;
      formatData->greenMask  = SK_G16_MASK;
      formatData->blueMask   = SK_B16_MASK;
      formatData->alphaMask  = 0;
      break;
    case kARGB_4444_SkColorType:
      formatData->redShift   = SK_R4444_SHIFT;
      formatData->greenShift = SK_G4444_SHIFT;
      formatData->blueShift  = SK_B4444_SHIFT;
      formatData->alphaShift = SK_A4444_SHIFT;
      formatData->redMask    = (15 << SK_R4444_SHIFT);
      formatData->greenMask  = (15 << SK_G4444_SHIFT);
      formatData->blueMask   = (15 << SK_B4444_SHIFT);
      formatData->alphaMask  = (15 << SK_A4444_SHIFT);
      break;
    case kRGBA_8888_SkColorType:
      formatData->redShift   = SK_RGBA_R32_SHIFT;
      formatData->greenShift = SK_RGBA_G32_SHIFT;
      formatData->blueShift  = SK_RGBA_B32_SHIFT;
      formatData->alphaShift = SK_RGBA_A32_SHIFT;
      formatData->redMask    = (255 << SK_RGBA_R32_SHIFT);
      formatData->greenMask  = (255 << SK_RGBA_G32_SHIFT);
      formatData->blueMask   = (255 << SK_RGBA_B32_SHIFT);
      formatData->alphaMask  = (255 << SK_RGBA_A32_SHIFT);
      break;
    case kBGRA_8888_SkColorType:
      formatData->redShift   = SK_BGRA_R32_SHIFT;
      formatData->greenShift = SK_BGRA_G32_SHIFT;
      formatData->blueShift  = SK_BGRA_B32_SHIFT;
      formatData->alphaShift = SK_BGRA_A32_SHIFT;
      formatData->redMask    = (255 << SK_BGRA_R32_SHIFT);
      formatData->greenMask  = (255 << SK_BGRA_G32_SHIFT);
      formatData->blueMask   = (255 << SK_BGRA_B32_SHIFT);
      formatData->alphaMask  = (255 << SK_BGRA_A32_SHIFT);
      break;
    default:
      formatData->redShift   = 0;
      formatData->greenShift = 0;
      formatData->blueShift  = 0;
      formatData->alphaShift = 0;
      formatData->redMask    = 0;
      formatData->greenMask  = 0;
      formatData->blueMask   = 0;
      formatData->alphaMask  = 0;
      break;
  }
}

gfx::Color SkiaSurface::getPixel(int x, int y) const
{
  // Clip input to avoid crash on SkBitmap::getColor()
  if (x < 0 || y < 0 || x >= width() || y >= height())
    return 0;

  SkColor c = 0;

  if (m_surface) {
    SkImageInfo dstInfo = SkImageInfo::MakeN32Premul(1, 1, skColorSpace());
    uint32_t dstPixels;
    if (m_canvas->readPixels(dstInfo, &dstPixels, 4, x, y))
      c = dstPixels;
  }
  else
    c = m_bitmap.getColor(x, y);

  return gfx::rgba(
    SkColorGetR(c),
    SkColorGetG(c),
    SkColorGetB(c),
    SkColorGetA(c));
}

void SkiaSurface::putPixel(gfx::Color color, int x, int y)
{
  if (m_surface) {
    m_paint.setColor(to_skia(color));
    m_canvas->drawPoint(SkIntToScalar(x), SkIntToScalar(y), m_paint);
  }
  else {
    // TODO Find a better way to put a pixel in the same color space
    //      as the internal SkPixmap (as Skia expects a sRGB color
    //      in SkBitmap::erase())
#if 1
    auto r = SkIRect::MakeXYWH(x, y, 1, 1);
    const_cast<SkPixmap&>(m_bitmap.pixmap()).erase(
      to_skia4f(color),
      skColorSpace().get(),
      &r);
#else
    // Doesn't work as SkBitmap::erase() expects an sRGB color (and
    // the color is should be in the same color space as this
    // surface, so there is no conversion).
    m_bitmap.erase(to_skia(color),
                   SkIRect::MakeXYWH(x, y, 1, 1));
#endif
  }
}

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

void SkiaSurface::blitTo(Surface* _dst, int srcx, int srcy, int dstx, int dsty, int width, int height) const
{
  auto dst = static_cast<SkiaSurface*>(_dst);

  SkRect srcRect = SkRect::MakeXYWH(srcx, srcy, width, height);
  SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(dstx, dsty, width, height));

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrc);

  if (!m_bitmap.empty()) {
    dst->m_canvas->drawImageRect(
      SkImage::MakeFromRaster(m_bitmap.pixmap(), nullptr, nullptr),
      srcRect, dstRect,
      SkSamplingOptions(),
      &paint, SkCanvas::kStrict_SrcRectConstraint);
  }
  else {
    sk_sp<SkImage> snapshot = m_surface->makeImageSnapshot(srcRect.round());
    srcRect.offsetTo(0, 0);
    dst->m_canvas->drawImageRect(
      snapshot, srcRect, dstRect,
      SkSamplingOptions(),
      &paint, SkCanvas::kStrict_SrcRectConstraint);
  }
}

void SkiaSurface::scrollTo(const gfx::Rect& rc, int dx, int dy)
{
  int w = width();
  int h = height();
  gfx::Clip clip(rc.x+dx, rc.y+dy, rc);
  if (!clip.clip(w, h, w, h))
    return;

  if (m_surface) {
    SurfaceLock lock(this);
    blitTo(this, clip.src.x, clip.src.y, clip.dst.x, clip.dst.y, clip.size.w, clip.size.h);
    return;
  }

  int bytesPerPixel = m_bitmap.bytesPerPixel();
  int rowBytes = (int)m_bitmap.rowBytes();
  int rowDelta;

  if (dy > 0) {
    clip.src.y += clip.size.h-1;
    clip.dst.y += clip.size.h-1;
    rowDelta = -rowBytes;
  }
  else
    rowDelta = rowBytes;

  char* dst = (char*)m_bitmap.getPixels();
  const char* src = dst;
  dst += rowBytes*clip.dst.y + bytesPerPixel*clip.dst.x;
  src += rowBytes*clip.src.y + bytesPerPixel*clip.src.x;
  w = bytesPerPixel*clip.size.w;
  h = clip.size.h;

  while (--h >= 0) {
    memmove(dst, src, w);
    dst += rowDelta;
    src += rowDelta;
  }

  m_bitmap.notifyPixelsChanged();
}

void SkiaSurface::drawSurface(const Surface* src, int dstx, int dsty)
{
  gfx::Clip clip(dstx, dsty, 0, 0, src->width(), src->height());
  if (!clip.clip(width(), height(), clip.size.w, clip.size.h))
    return;

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrc);
  skDrawSurface(
    src,
    clip,
    SkSamplingOptions(),
    paint);
}

void SkiaSurface::drawSurface(const Surface* src,
                              const gfx::Rect& srcRect,
                              const gfx::Rect& dstRect,
                              const Sampling& sampling,
                              const os::Paint* paint)
{
  SkPaint skPaint;
  if (paint)
    to_skia(*paint, skPaint);
  else
    skPaint.setBlendMode(SkBlendMode::kSrc);

  SkSamplingOptions skSampling;
  to_skia(sampling, skSampling);

  skDrawSurface(
    src,
    srcRect,
    dstRect,
    skSampling,
    skPaint);
}

void SkiaSurface::drawRgbaSurface(const Surface* src, int dstx, int dsty)
{
  gfx::Clip clip(dstx, dsty, 0, 0, src->width(), src->height());
  if (!clip.clip(width(), height(), clip.size.w, clip.size.h))
    return;

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrcOver);
  skDrawSurface(
    src,
    clip,
    SkSamplingOptions(),
    paint);
}

void SkiaSurface::drawRgbaSurface(const Surface* src, int srcx, int srcy, int dstx, int dsty, int w, int h)
{
  gfx::Clip clip(dstx, dsty, srcx, srcy, w, h);
  if (!clip.clip(width(), height(), src->width(), src->height()))
    return;

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrcOver);
  skDrawSurface(
    src,
    clip,
    SkSamplingOptions(),
    paint);
}

void SkiaSurface::drawColoredRgbaSurface(const Surface* src, gfx::Color fg, gfx::Color bg, const gfx::Clip& clipbase)
{
  gfx::Clip clip(clipbase);
  if (!clip.clip(width(), height(), src->width(), src->height()))
    return;

  SkRect srcRect = SkRect::Make(SkIRect::MakeXYWH(clip.src.x, clip.src.y, clip.size.w, clip.size.h));
  SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(clip.dst.x, clip.dst.y, clip.size.w, clip.size.h));

  SkPaint paint;
  paint.setBlendMode(SkBlendMode::kSrcOver);

  if (gfx::geta(bg) > 0) {
    SkPaint paint;
    paint.setColor(to_skia(bg));
    paint.setStyle(SkPaint::kFill_Style);
    m_canvas->drawRect(dstRect, paint);
  }

  sk_sp<SkColorFilter> colorFilter(
    SkColorFilters::Blend(to_skia(fg), SkBlendMode::kSrcIn));
  paint.setColorFilter(colorFilter);

  skDrawSurface(
    (SkiaSurface*)src,
    srcRect,
    dstRect,
    SkSamplingOptions(),
    paint);
}

void SkiaSurface::drawSurfaceNine(os::Surface* surface,
                                  const gfx::Rect& src,
                                  const gfx::Rect& _center,
                                  const gfx::Rect& dst,
                                  const os::Paint* paint)
{
  gfx::Rect center(_center);
  SkIRect srcRect = SkIRect::MakeXYWH(src.x, src.y, src.w, src.h);
  SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(dst.x, dst.y, dst.w, dst.h));

  SkPaint skPaint;
  skPaint.setBlendMode(SkBlendMode::kSrcOver);
  if (paint && paint->color() != gfx::ColorNone) {
    sk_sp<SkColorFilter> colorFilter(
      SkColorFilters::Blend(to_skia(paint->color()),
                            SkBlendMode::kSrcIn));
    skPaint.setColorFilter(colorFilter);
  }

  int xdivs[2] = { src.x+center.x, src.x+center.x2() };
  int ydivs[2] = { src.y+center.y, src.y+center.y2() };
  SkCanvas::Lattice::RectType rectTypes[9] =
    { SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault,
      (paint && paint->hasFlags(Paint::kNineWithoutCenter) ?
       SkCanvas::Lattice::kTransparent:
       SkCanvas::Lattice::kDefault),
      SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault,
      SkCanvas::Lattice::kDefault };

  // Without left side
  if (center.x == 0) {
    srcRect.fLeft -= 1;
    dstRect.fLeft -= 1;
    rectTypes[0] =
      rectTypes[3] =
      rectTypes[6] = SkCanvas::Lattice::kTransparent;
  }

  // Without right side
  if (center.x2() == src.w) {
    srcRect.fRight += 1;
    dstRect.fRight += 1;
    rectTypes[2] =
      rectTypes[5] =
      rectTypes[8] = SkCanvas::Lattice::kTransparent;
  }

  // Without top side
  if (center.y == 0) {
    srcRect.fTop -= 1;
    dstRect.fTop -= 1;
    rectTypes[0] =
      rectTypes[1] =
      rectTypes[2] = SkCanvas::Lattice::kTransparent;
  }

  // Without bottom side
  if (center.y2() == src.h) {
    srcRect.fBottom += 1;
    srcRect.fBottom += 1;
    rectTypes[6] =
      rectTypes[7] =
      rectTypes[8] = SkCanvas::Lattice::kTransparent;
  }

  SkCanvas::Lattice lattice;
  lattice.fXDivs = xdivs;
  lattice.fYDivs = ydivs;
  lattice.fRectTypes = rectTypes;
  lattice.fXCount = 2;
  lattice.fYCount = 2;
  lattice.fBounds = &srcRect;
  lattice.fColors = nullptr;

  m_canvas->drawImageLattice(
    SkImage::MakeFromRaster(((SkiaSurface*)surface)->m_bitmap.pixmap(), nullptr, nullptr).get(),
    lattice, dstRect,
    SkFilterMode::kNearest,
    &skPaint);
}

void SkiaSurface::swapBitmap(SkBitmap& other)
{
  ASSERT(!m_surface);
  m_bitmap.swap(other);
  delete m_canvas;
  m_canvas = new SkCanvas(m_bitmap);
}

// static
Ref<Surface> SkiaSurface::loadSurface(const char* filename)
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

  auto sur = make_ref<SkiaSurface>();
  sur->swapBitmap(bm);
  return sur;
}

void SkiaSurface::skDrawSurface(
  const Surface* src,
  const gfx::Clip& clip,
  const SkSamplingOptions& sampling,
  const SkPaint& paint)
{
  skDrawSurface(static_cast<const SkiaSurface*>(src),
                SkRect::MakeXYWH(clip.src.x, clip.src.y, clip.size.w, clip.size.h),
                SkRect::MakeXYWH(clip.dst.x, clip.dst.y, clip.size.w, clip.size.h),
                sampling,
                paint);
}

void SkiaSurface::skDrawSurface(
  const Surface* src,
  const gfx::Rect& srcRect,
  const gfx::Rect& dstRect,
  const SkSamplingOptions& sampling,
  const SkPaint& paint)
{
  skDrawSurface(static_cast<const SkiaSurface*>(src),
                SkRect::MakeXYWH(srcRect.x, srcRect.y, srcRect.w, srcRect.h),
                SkRect::MakeXYWH(dstRect.x, dstRect.y, dstRect.w, dstRect.h),
                sampling,
                paint);
}

void SkiaSurface::skDrawSurface(
  const SkiaSurface* src,
  const SkRect& srcRect,
  const SkRect& dstRect,
  const SkSamplingOptions& sampling,
  const SkPaint& paint)
{
  m_canvas->drawImageRect(
    SkImage::MakeFromRaster(src->m_bitmap.pixmap(), nullptr, nullptr),
    srcRect,
    dstRect,
    sampling,
    &paint,
    SkCanvas::kStrict_SrcRectConstraint);
}

} // namespace os
