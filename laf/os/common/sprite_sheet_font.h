// LAF OS Library
// Copyright (C) 2019-2023  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SPRITE_SHEET_FONT_H
#define OS_SPRITE_SHEET_FONT_H
#pragma once

#include "base/debug.h"
#include "base/string.h"
#include "base/utf8_decode.h"
#include "gfx/rect.h"
#include "os/font.h"
#include "os/ref.h"
#include "os/surface.h"

#include <vector>

namespace os {

class SpriteSheetFont : public Font {
  static constexpr auto kRedColor = gfx::rgba(255, 0, 0);

public:
  SpriteSheetFont() : m_sheet(nullptr) { }
  ~SpriteSheetFont() { }

  FontType type() override {
    return FontType::SpriteSheet;
  }

  int height() const override {
    return getCharBounds(' ').h;
  }

  int textLength(const std::string& str) const override {
    base::utf8_decode decode(str);
    int x = 0;
    while (int chr = decode.next())
      x += getCharBounds(chr).w;
    return x;
  }

  bool isScalable() const override {
    return false;
  }

  void setSize(int size) override {
    // Do nothing
  }

  void setAntialias(bool antialias) override {
    // Do nothing
  }

  bool hasCodePoint(int codepoint) const override {
    codepoint -= (int)' ';
    return (codepoint >= 0 &&
            codepoint < (int)m_chars.size() &&
            !m_chars[codepoint].isEmpty());
  }

  Surface* sheetSurface() const {
    return m_sheet.get();
  }

  gfx::Rect getCharBounds(int chr) const {
    chr -= (int)' ';
    if (chr >= 0 && chr < (int)m_chars.size())
      return m_chars[chr];
    else if (chr != 128)
      return getCharBounds(128);
    else
      return gfx::Rect();
  }

  static FontRef fromSurface(const SurfaceRef& sur) {
    auto font = make_ref<SpriteSheetFont>();
    font->m_sheet = sur;

    SurfaceLock lock(sur.get());
    gfx::Rect bounds(0, 0, 1, 1);
    gfx::Rect charBounds;

    while (font->findChar(sur.get(), sur->width(), sur->height(), bounds, charBounds)) {
      font->m_chars.push_back(charBounds);
      bounds.x += bounds.w;
    }

    return font;
  }

private:

  bool findChar(const Surface* sur, int width, int height,
                gfx::Rect& bounds, gfx::Rect& charBounds) {
    gfx::Color keyColor = sur->getPixel(0, 0);

    while (sur->getPixel(bounds.x, bounds.y) == keyColor) {
      bounds.x++;
      if (bounds.x >= width) {
        bounds.x = 0;
        bounds.y += bounds.h;
        bounds.h = 1;
        if (bounds.y >= height)
          return false;
      }
    }

    gfx::Color firstCharPixel = sur->getPixel(bounds.x, bounds.y);

    bounds.w = 0;
    while ((bounds.x+bounds.w < width) &&
           (sur->getPixel(bounds.x+bounds.w, bounds.y) != keyColor)) {
      bounds.w++;
    }

    bounds.h = 0;
    while ((bounds.y+bounds.h < height) &&
           (sur->getPixel(bounds.x, bounds.y+bounds.h) != keyColor)) {
      bounds.h++;
    }

    // Using red color in the first pixel of the char indicates that
    // this glyph shouldn't be used as a valid one.
    if (firstCharPixel != kRedColor)
      charBounds = bounds;
    else
      charBounds = gfx::Rect();

    return !bounds.isEmpty();
  }

private:
  Ref<Surface> m_sheet;
  std::vector<gfx::Rect> m_chars;
};

} // namespace os

#endif
