// LAF Gfx Library
// Copyright (C) 2019  Igara Studio S.A.
// Copyright (C) 2001-2014 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gfx/packing_rects.h"

#include "gfx/region.h"
#include "gfx/size.h"

namespace gfx {

void PackingRects::add(const Size& sz)
{
  m_rects.push_back(Rect(sz));
}

void PackingRects::add(const Rect& rc)
{
  m_rects.push_back(rc);
}

Size PackingRects::bestFit(base::task_token& token,
                           const int fixedWidth,
                           const int fixedHeight)
{
  Size size(fixedWidth, fixedHeight);

  // Nothing to do, the size is already specified
  if (fixedWidth > 0 && fixedHeight > 0)
    return size;

  // Calculate the amount of pixels that we need, the texture cannot
  // be smaller than that.
  int neededArea = 0;
  for (const auto& rc : m_rects) {
    neededArea += rc.w * rc.h;
    size |= rc.size();
  }

  const int w0 = std::max(size.w, 1);
  const int h0 = std::max(size.h, 1);
  int w = w0;
  int h = h0;
  int z = 0;
  bool fit = false;
  while (!token.canceled()) {
    if (w*h >= neededArea) {
      fit = pack(Size(w, h), token);
      if (fit) {
        size = Size(w, h);
        break;
      }
    }

    if (fixedWidth == 0 && fixedHeight == 0) {
      if ((++z) & 1)
        w += w0;
      else
        h += h0;
    }
    else if (fixedWidth == 0) {
      w += w0;
    }
    else {
      h += h0;
    }
  }

  return size;
}

static bool by_area(const Rect* a, const Rect* b) {
  return a->w*a->h > b->w*b->h;
}

bool PackingRects::pack(const Size& size,
                        base::task_token& token)
{
  m_bounds = Rect(size).shrink(m_borderPadding);

  // We cannot sort m_rects because we want to
  std::vector<Rect*> rectPtrs(m_rects.size());
  int i = 0;
  for (auto& rc : m_rects)
    rectPtrs[i++] = &rc;
  std::sort(rectPtrs.begin(), rectPtrs.end(), by_area);

  gfx::Region rgn(m_bounds);
  i = 0;
  for (auto rcPtr : rectPtrs) {
    if (token.canceled())
      return false;
    token.set_progress(float(i) / int(rectPtrs.size()));

    gfx::Rect& rc = *rcPtr;

    // The rectangles are treated as its original size during placement,
    // but occupies an extra border of <shapePadding> pixels once its
    // position has been determined.
    // This ensures that all rectangles are padded by <sp> pixels,
    // and are still placed correctly near edges, e.g. when remaining
    // horizontal space is between <width> and <width>+<shapePadding>.
    for (int v=0; v<=m_bounds.h-rc.h; ++v) {
      for (int u=0; u<=m_bounds.w-rc.w; ++u) {
        if (token.canceled())
          return false;

        gfx::Rect possible(m_bounds.x + u, m_bounds.y + v, rc.w, rc.h);
        Region::Overlap overlap = rgn.contains(possible);
        if (overlap == Region::In) {
          rc = possible;
          rgn.createSubtraction(
            rgn,
            gfx::Region(Rect(rc).inflate(m_shapePadding))
          );
          goto next_rc;
        }
      }
    }
    return false; // There is not enough room for "rc"
  next_rc:;
    ++i;
  }

  return true;
}

} // namespace gfx
