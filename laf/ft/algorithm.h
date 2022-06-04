// LAF FreeType Wrapper
// Copyright (c) 2022 Igara Studio S.A.
// Copyright (c) 2016-2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef FT_ALGORITHM_H_INCLUDED
#define FT_ALGORITHM_H_INCLUDED
#pragma once

#include "base/string.h"
#include "base/utf8_decode.h"
#include "ft/freetype_headers.h"
#include "ft/hb_shaper.h"
#include "gfx/rect.h"

namespace ft {

  template<typename FaceFT>
  class DefaultShaper {
  public:
    using Glyph = typename FaceFT::Glyph;

    DefaultShaper(FaceFT& face,
                  const std::string& str)
      : m_face(face)
      , m_begin(str.begin())
      , m_decode(str) {
    }

    int next() {
      m_pos = m_decode.pos();
      return (m_char = m_decode.next());
    }

    int unicodeChar() const {
      return m_char;
    }

    int charIndex() const {
      return m_pos - m_begin;
    }

    unsigned int glyphIndex() {
      return m_face.cache().getGlyphIndex(m_face, unicodeChar());
    }

    void glyphOffsetXY(Glyph* glyph) {
      // Do nothing
    }

    void glyphAdvanceXY(const Glyph* glyph, double& x, double& y) {
      x += glyph->ft_glyph->advance.x / double(1 << 16);
      y += glyph->ft_glyph->advance.y / double(1 << 16);
    }

  private:
    FaceFT& m_face;
    std::string::const_iterator m_begin;
    std::string::const_iterator m_pos;
    base::utf8_decode m_decode;
    int m_char = 0;
  };

  template<typename FaceFT,
           typename Shaper = HBShaper<FaceFT> >
  class ForEachGlyph {
  public:
    typedef typename FaceFT::Glyph Glyph;

    ForEachGlyph(FaceFT& face, const std::string& str)
      : m_face(face)
      , m_shaper(face, str)
      , m_glyph(nullptr)
      , m_useKerning(FT_HAS_KERNING(((FT_Face)face)) ? true: false)
      , m_prevGlyph(0)
      , m_x(0.0), m_y(0.0) {
    }

    ~ForEachGlyph() {
      unloadGlyph();
    }

    int unicodeChar() { return m_shaper.unicodeChar(); }
    int charIndex() { return m_shaper.charIndex(); }

    const Glyph* glyph() const { return m_glyph; }

    int next() {
      if (m_glyph)
        m_prevGlyph = m_shaper.glyphIndex();

      if (int chr = m_shaper.next()) {
        prepareGlyph();
        return chr;
      }
      else
        return 0;
    }

  private:
    void prepareGlyph() {
      FT_UInt glyphIndex = m_shaper.glyphIndex();
      double initialX = m_x;

      if (m_useKerning && m_prevGlyph && glyphIndex) {
        FT_Vector kerning;
        FT_Get_Kerning(m_face, m_prevGlyph, glyphIndex,
                       FT_KERNING_DEFAULT, &kerning);
        m_x += kerning.x / 64.0;
      }

      unloadGlyph();

      // Load new glyph
      m_glyph = m_face.cache().loadGlyph(m_face, glyphIndex, m_face.antialias());
      if (m_glyph) {
        m_glyph->bitmap = &FT_BitmapGlyph(m_glyph->ft_glyph)->bitmap;
        m_glyph->x = m_x
          + m_glyph->bearingX;
        m_glyph->y = m_y
          + m_face.height()
          + m_face.descender() // descender is negative
          - m_glyph->bearingY;

        m_shaper.glyphOffsetXY(m_glyph);
        m_shaper.glyphAdvanceXY(m_glyph, m_x, m_y);

        m_glyph->startX = initialX;
        m_glyph->endX = m_x;
      }
    }

    void unloadGlyph() {
      if (m_glyph) {
        m_face.cache().doneGlyph(m_glyph);
        m_glyph = nullptr;
      }
    }

  private:
    FaceFT& m_face;
    Shaper m_shaper;
    Glyph* m_glyph;
    bool m_useKerning;
    FT_UInt m_prevGlyph;
    double m_x, m_y;
  };

  template<typename FaceFT>
  gfx::Rect calc_text_bounds(FaceFT& face, const std::string& str) {
    gfx::Rect bounds(0, 0, 0, 0);
    ForEachGlyph<FaceFT> feg(face, str);
    while (feg.next()) {
      if (auto glyph = feg.glyph())
        bounds |= gfx::Rect(int(glyph->x),
                            int(glyph->y),
                            glyph->bitmap->width,
                            glyph->bitmap->rows);
    }
    return bounds;
  }

} // namespace ft

#endif
