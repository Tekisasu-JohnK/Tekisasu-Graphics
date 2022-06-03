// LAF OS Library
// Copyright (c) 2018-2020  Igara Studio S.A.
// Copyright (c) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_SURFACE_INCLUDED
#define OS_SKIA_SKIA_SURFACE_INCLUDED
#pragma once

#include "base/exception.h"
#include "gfx/clip.h"
#include "gfx/matrix.h"
#include "os/common/generic_surface.h"
#include "os/common/sprite_sheet_font.h"
#include "os/skia/skia_color_space.h"
#include "os/skia/skia_helpers.h"

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "SkColorFilter.h"
#include "SkColorPriv.h"
#include "SkImageInfo.h"
#include "SkRegion.h"
#include "SkSurface.h"

#include "include/private/SkColorData.h"

#include <atomic>

namespace os {

class SkiaSurface : public Surface {
public:
  SkiaSurface() : m_surface(nullptr)
                , m_colorSpace(nullptr)
                , m_canvas(nullptr)
                , m_lock(0) {
  }

  SkiaSurface(const sk_sp<SkSurface>& surface)
    : m_surface(surface)
    , m_colorSpace(nullptr)
    , m_canvas(nullptr)
    , m_lock(0)
  {
    ASSERT(m_surface);
    if (m_surface)
      m_canvas = m_surface->getCanvas();
  }

  ~SkiaSurface() {
    ASSERT(m_lock == 0);
    if (!m_surface)
      delete m_canvas;
  }

  void create(int width, int height,
              const os::ColorSpacePtr& colorSpace) {
    ASSERT(!m_surface);
    ASSERT(width > 0);
    ASSERT(height > 0);

    m_colorSpace = colorSpace;

    if (!m_bitmap.tryAllocPixels(
          SkImageInfo::MakeN32(width, height, kOpaque_SkAlphaType, skColorSpace())))
      throw base::Exception("Cannot create Skia surface");

    m_bitmap.eraseColor(SK_ColorTRANSPARENT);
    rebuild();
  }

  void createRgba(int width, int height,
                  const os::ColorSpacePtr& colorSpace) {
    ASSERT(!m_surface);
    ASSERT(width > 0);
    ASSERT(height > 0);

    m_colorSpace = colorSpace;

    if (!m_bitmap.tryAllocPixels(
          SkImageInfo::MakeN32Premul(width, height, skColorSpace())))
      throw base::Exception("Cannot create Skia surface");

    m_bitmap.eraseColor(SK_ColorTRANSPARENT);
    rebuild();
  }

  void flush() const {
    if (m_canvas)
      m_canvas->flush();
  }

  // Surface impl

  void dispose() override {
    delete this;
  }

  int width() const override {
    if (m_surface)
      return m_surface->width();
    else
      return m_bitmap.width();
  }

  int height() const override {
    if (m_surface)
      return m_surface->height();
    else
      return m_bitmap.height();
  }

  const ColorSpacePtr& colorSpace() const override {
    return m_colorSpace;
  }

  bool isDirectToScreen() const override {
    return false;
  }

  int getSaveCount() const override {
    return m_canvas->getSaveCount();
  }

  gfx::Rect getClipBounds() const override {
    SkIRect rc;
    if (m_canvas->getDeviceClipBounds(&rc))
      return gfx::Rect(rc.x(), rc.y(), rc.width(), rc.height());
    else
      return gfx::Rect();
  }

  void saveClip() override {
    m_canvas->save();
  }

  void restoreClip() override {
    m_canvas->restore();
  }

  bool clipRect(const gfx::Rect& rc) override {
    m_canvas->clipRect(SkRect::Make(to_skia(rc)));
    return !m_canvas->isClipEmpty();
  }

  void save() override {
    m_canvas->save();
  }

  void concat(const gfx::Matrix& matrix) override {
    m_canvas->concat(matrix.skMatrix());
  }

  void setMatrix(const gfx::Matrix& matrix) override {
    m_canvas->setMatrix(matrix.skMatrix());
  }

  void resetMatrix() override {
    m_canvas->resetMatrix();
  }

  void restore() override {
    m_canvas->restore();
  }

  gfx::Matrix matrix() const override {
    return m_canvas->getTotalMatrix();
  }

  void setDrawMode(DrawMode mode, int param,
                   const gfx::Color a,
                   const gfx::Color b) override {
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
                              SkTileMode::kRepeat));
          m_paint.setShader(shader);
        }
        break;
      }
    }
  }

  void lock() override {
    ASSERT(m_lock >= 0);
    if (m_lock++ == 0) {
      // TODO add mutex!
      // m_bitmap is always locked
    }
  }

  void unlock() override {
    ASSERT(m_lock > 0);
    if (--m_lock == 0) {
      // m_bitmap is always locked
    }
  }

  void applyScale(int scaleFactor) override {
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
    canvas.drawBitmapRect(m_bitmap, srcRect, dstRect, &paint,
                          SkCanvas::kStrict_SrcRectConstraint);

    swapBitmap(result);
  }

  void* nativeHandle() override {
    return (void*)this;
  }

  void clear() override {
    m_canvas->clear(0);
  }

  uint8_t* getData(int x, int y) const override {
    if (m_bitmap.isNull())
      return nullptr;
    else
      return (uint8_t*)m_bitmap.getAddr32(x, y);
  }

  void getFormat(SurfaceFormatData* formatData) const override {
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

  gfx::Color getPixel(int x, int y) const override {
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

  void putPixel(gfx::Color color, int x, int y) override {
    if (m_surface) {
      m_paint.setColor(to_skia(color));
      m_canvas->drawPoint(SkIntToScalar(x), SkIntToScalar(y), m_paint);
    }
    else {
      m_bitmap.erase(to_skia(color), SkIRect::MakeXYWH(x, y, 1, 1));
    }
  }

  void drawLine(const float x0, const float y0,
                const float x1, const float y1,
                const Paint& paint) override;

  void drawRect(const gfx::RectF& rc,
                const Paint& paint) override;

  void drawCircle(const float cx, const float cy,
                  const float radius,
                  const Paint& paint) override;

  void drawPath(const gfx::Path& path,
                const Paint& paint) override;

  void blitTo(Surface* _dst, int srcx, int srcy, int dstx, int dsty, int width, int height) const override {
    auto dst = static_cast<SkiaSurface*>(_dst);

    SkIRect srcRect = SkIRect::MakeXYWH(srcx, srcy, width, height);
    SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(dstx, dsty, width, height));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrc);

    if (!m_bitmap.empty())
      dst->m_canvas->drawBitmapRect(m_bitmap, srcRect, dstRect, &paint,
                                    SkCanvas::kStrict_SrcRectConstraint);
    else {
      sk_sp<SkImage> snapshot = m_surface->makeImageSnapshot(srcRect);
      srcRect.offsetTo(0, 0);
      dst->m_canvas->drawImageRect(snapshot, srcRect, dstRect, &paint,
                                   SkCanvas::kStrict_SrcRectConstraint);
    }
  }

  void scrollTo(const gfx::Rect& rc, int dx, int dy) override {
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

  void drawSurface(const Surface* src, int dstx, int dsty) override {
    gfx::Clip clip(dstx, dsty, 0, 0,
      ((SkiaSurface*)src)->width(),
      ((SkiaSurface*)src)->height());

    if (!clip.clip(width(), height(), clip.size.w, clip.size.h))
      return;

    SkRect srcRect = SkRect::Make(SkIRect::MakeXYWH(clip.src.x, clip.src.y, clip.size.w, clip.size.h));
    SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(clip.dst.x, clip.dst.y, clip.size.w, clip.size.h));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrc);

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap, srcRect, dstRect, &paint,
      SkCanvas::kStrict_SrcRectConstraint);
  }

  void drawSurface(const Surface* src, const gfx::Rect& srcRect, const gfx::Rect& dstRect) override {
    SkRect srcRect2 = SkRect::Make(SkIRect::MakeXYWH(srcRect.x, srcRect.y, srcRect.w, srcRect.h));
    SkRect dstRect2 = SkRect::Make(SkIRect::MakeXYWH(dstRect.x, dstRect.y, dstRect.w, dstRect.h));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrc);
    paint.setFilterQuality(srcRect.w < dstRect.w ||
                           srcRect.h < dstRect.h ? kNone_SkFilterQuality:
                                                   kHigh_SkFilterQuality);

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap, srcRect2, dstRect2, &paint,
      SkCanvas::kStrict_SrcRectConstraint);
  }

  void drawRgbaSurface(const Surface* src, int dstx, int dsty) override {
    gfx::Clip clip(dstx, dsty, 0, 0,
      ((SkiaSurface*)src)->width(),
      ((SkiaSurface*)src)->height());

    if (!clip.clip(width(), height(), clip.size.w, clip.size.h))
      return;

    SkRect srcRect = SkRect::Make(SkIRect::MakeXYWH(clip.src.x, clip.src.y, clip.size.w, clip.size.h));
    SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(clip.dst.x, clip.dst.y, clip.size.w, clip.size.h));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrcOver);

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap, srcRect, dstRect, &paint,
      SkCanvas::kStrict_SrcRectConstraint);
  }

  void drawRgbaSurface(const Surface* src, int srcx, int srcy, int dstx, int dsty, int w, int h) override {
    gfx::Clip clip(dstx, dsty, srcx, srcy, w, h);
    if (!clip.clip(width(), height(), src->width(), src->height()))
      return;

    SkRect srcRect = SkRect::Make(SkIRect::MakeXYWH(clip.src.x, clip.src.y, clip.size.w, clip.size.h));
    SkRect dstRect = SkRect::Make(SkIRect::MakeXYWH(clip.dst.x, clip.dst.y, clip.size.w, clip.size.h));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrcOver);

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap, srcRect, dstRect, &paint,
      SkCanvas::kStrict_SrcRectConstraint);
  }

  void drawRgbaSurface(const Surface* src, const gfx::Rect& srcRect, const gfx::Rect& dstRect) override {
    SkRect srcRect2 = SkRect::Make(SkIRect::MakeXYWH(srcRect.x, srcRect.y, srcRect.w, srcRect.h));
    SkRect dstRect2 = SkRect::Make(SkIRect::MakeXYWH(dstRect.x, dstRect.y, dstRect.w, dstRect.h));

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrcOver);
    paint.setFilterQuality(srcRect.w < dstRect.w ||
                           srcRect.h < dstRect.h ? kNone_SkFilterQuality:
                                                   kHigh_SkFilterQuality);

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap, srcRect2, dstRect2, &paint,
      SkCanvas::kStrict_SrcRectConstraint);
  }

  void drawColoredRgbaSurface(const Surface* src, gfx::Color fg, gfx::Color bg, const gfx::Clip& clipbase) override {
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

    m_canvas->drawBitmapRect(
      ((SkiaSurface*)src)->m_bitmap,
      srcRect, dstRect, &paint);
  }

  void drawSurfaceNine(os::Surface* surface,
                       const gfx::Rect& src,
                       const gfx::Rect& _center,
                       const gfx::Rect& dst,
                       const os::Paint* paint) override {
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
      srcRect.fLeft -= SkIntToScalar(1);
      dstRect.fLeft -= SkIntToScalar(1);
      rectTypes[0] =
      rectTypes[3] =
      rectTypes[6] = SkCanvas::Lattice::kTransparent;
    }

    // Without right side
    if (center.x2() == src.w) {
      srcRect.fRight += SkIntToScalar(1);
      dstRect.fRight += SkIntToScalar(1);
      rectTypes[2] =
      rectTypes[5] =
      rectTypes[8] = SkCanvas::Lattice::kTransparent;
    }

    // Without top side
    if (center.y == 0) {
      srcRect.fTop -= SkIntToScalar(1);
      dstRect.fTop -= SkIntToScalar(1);
      rectTypes[0] =
      rectTypes[1] =
      rectTypes[2] = SkCanvas::Lattice::kTransparent;
    }

    // Without bottom side
    if (center.y2() == src.h) {
      srcRect.fBottom += SkIntToScalar(1);
      srcRect.fBottom += SkIntToScalar(1);
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

    m_canvas->drawBitmapLattice(
      ((SkiaSurface*)surface)->m_bitmap,
      lattice, dstRect, &skPaint);
  }

  SkBitmap& bitmap() { return m_bitmap; }
  SkCanvas& canvas() { return *m_canvas; }

  void swapBitmap(SkBitmap& other) {
    ASSERT(!m_surface);

    m_bitmap.swap(other);
    rebuild();
  }

  static Surface* loadSurface(const char* filename);

private:
  void rebuild() {
    ASSERT(!m_surface);

    delete m_canvas;
    m_canvas = new SkCanvas(m_bitmap);
  }

  sk_sp<SkColorSpace> skColorSpace() const {
    if (m_colorSpace)
      return static_cast<SkiaColorSpace*>(m_colorSpace.get())->skColorSpace();
    else
      return nullptr;
  }

  SkBitmap m_bitmap;
  sk_sp<SkSurface> m_surface;
  ColorSpacePtr m_colorSpace;
  SkCanvas* m_canvas;
  SkPaint m_paint;
  std::atomic<int> m_lock;

};

} // namespace os

#endif
