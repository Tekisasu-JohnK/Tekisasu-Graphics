// LAF OS Library
// Copyright (c) 2022  Igara Studio S.A.
// Copyright (C) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_DRAW_TEXT_H_INCLUDED
#define OS_DRAW_TEXT_H_INCLUDED
#pragma once

#include "base/string.h"
#include "gfx/color.h"
#include "gfx/fwd.h"

namespace os {

  class Font;
  class Surface;
  class Paint;

  enum class TextAlign { Left, Center, Right };

  class DrawTextDelegate {
  public:
    virtual ~DrawTextDelegate() { }

    // This is called before drawing the character.
    virtual void preProcessChar(const int index,
                                const int codepoint,
                                gfx::Color& fg,
                                gfx::Color& bg,
                                const gfx::Rect& charBounds) {
      // Do nothing
    }

    virtual bool preDrawChar(const gfx::Rect& charBounds) {
      // Returns false if the process should stop here.
      return true;
    }

    virtual void postDrawChar(const gfx::Rect& charBounds) {
      // Do nothing
    }
  };

  // The surface can be nullptr just to process the string
  // (e.g. measure how much space will use the text without drawing
  // it). It uses FreeType2 library and harfbuzz. Doesn't support RTL
  // (right-to-left) languages.
  gfx::Rect draw_text(
    Surface* surface, Font* font,
    const std::string& text,
    gfx::Color fg, gfx::Color bg,
    int x, int y,
    DrawTextDelegate* delegate);

  // Uses SkTextUtils::Draw() to draw text (doesn't depend on harfbuzz
  // or big dependencies, useful to print English text only).
  void draw_text(
    Surface* surface, Font* font,
    const std::string& text,
    const gfx::Point& pos,
    const Paint* paint = nullptr,
    const TextAlign textAlign = TextAlign::Left,
    DrawTextDelegate* delegate = nullptr);

  // Uses SkShaper::Make() to draw text (harfbuzz if available),
  // useful for RTL (right-to-left) languages. Avoid this function if
  // you are not going to translate your app to non-English languages
  // (prefer os::draw_text() when possible).
  void draw_text_with_shaper(
    Surface* surface, Font* font,
    const std::string& text,
    const gfx::Point& pos,
    const Paint* paint = nullptr,
    const TextAlign textAlign = TextAlign::Left,
    DrawTextDelegate* delegate = nullptr);

} // namespace os

#endif
