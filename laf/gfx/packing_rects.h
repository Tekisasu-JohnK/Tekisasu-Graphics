// LAF Gfx Library
// Copyright (C) 2019  Igara Studio S.A.
// Copyright (C) 2001-2015  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_TEXTURE_SIZE_H_INCLUDED
#define GFX_TEXTURE_SIZE_H_INCLUDED
#pragma once

#include "base/task.h"
#include "gfx/fwd.h"
#include "gfx/rect.h"
#include <vector>

namespace gfx {

  // TODO add support for rotations
  class PackingRects {
  public:
    PackingRects(int borderPadding = 0, int shapePadding = 0) :
      m_borderPadding(borderPadding),
      m_shapePadding(shapePadding) {
    }

    typedef std::vector<Rect> Rects;
    typedef Rects::const_iterator const_iterator;

    // Iterate over all given rectangles (in the same order they where
    // given in addSize() calls).
    const_iterator begin() const { return m_rects.begin(); }
    const_iterator end() const { return m_rects.end(); }

    std::size_t size() const { return m_rects.size(); }
    const Rect& operator[](int i) const { return m_rects[i]; }

    // Adds a new rectangle.
    void add(const Size& sz);
    void add(const Rect& rc);

    // Returns the best size for the texture.
    Size bestFit(base::task_token& token,
                 const int fixedWidth = 0,
                 const int fixedHeight = 0);

    // Rearrange all given rectangles to best fit a texture size.
    // Returns true if all rectangles were correctly arranged or false
    // if there is not enough space.
    bool pack(const Size& size,
              base::task_token& token);

    // Returns the bounds of the packed area.
    const Rect& bounds() const { return m_bounds; }

  private:
    int m_borderPadding;
    int m_shapePadding;

    Rect m_bounds;
    Rects m_rects;
  };

} // namespace gfx

#endif
