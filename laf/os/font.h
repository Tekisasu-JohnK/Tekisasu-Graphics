// LAF OS Library
// Copyright (c) 2019-2020  Igara Studio S.A.
// Copyright (c) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_FONT_H_INCLUDED
#define OS_FONT_H_INCLUDED
#pragma once

#include "base/ints.h"
#include "os/ref.h"

#include <string>

namespace os {

  class Font;
  class FontStyle;
  class Typeface;
  using FontRef = Ref<Font>;

  enum class FontType {
    Unknown,
    SpriteSheet,                // SpriteSheet
    FreeType,                   // FreeType
    Native,                     // Skia
  };

  class Font : public RefCount {
  public:
    Font() : m_fallback(nullptr) { }
    virtual ~Font() { }
    virtual FontType type() = 0;
    virtual int height() const = 0;
    virtual int textLength(const std::string& str) const = 0;
    virtual bool isScalable() const = 0;
    virtual void setSize(int size) = 0;
    virtual void setAntialias(bool antialias) = 0;
    virtual bool hasCodePoint(int codepoint) const = 0;

    Font* fallback() const {
      return m_fallback;
    }
    void setFallback(Font* font) {
      m_fallback = font;
    }

  private:
    Font* m_fallback;
  };

} // namespace os

#endif
