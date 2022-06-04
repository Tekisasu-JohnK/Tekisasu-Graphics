// LAF Gfx Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_PATH_SKIA_H_INCLUDED
#define GFX_PATH_SKIA_H_INCLUDED
#pragma once

#include "gfx/matrix.h"
#include "gfx/point.h"
#include "gfx/rect.h"

#include "include/core/SkPath.h"

namespace gfx {

  // Simple wrapper for SkPath
  // TODO add missing methods for curves
  class Path {
  public:
    Path() { }

    Path& reset() {
      m_skPath.reset();
      return *this;
    }

    Path& rewind() {
      m_skPath.rewind();
      return *this;
    }

    bool isEmpty() const {
      return m_skPath.isEmpty();
    }

    Path& moveTo(float x, float y) {
      m_skPath.moveTo(x, y);
      return *this;
    }

    Path& moveTo(const Point& p) {
      m_skPath.moveTo(p.x, p.y);
      return *this;
    }

    Path& lineTo(float x, float y) {
      m_skPath.lineTo(x, y);
      return *this;
    }

    Path& lineTo(const Point& p) {
      m_skPath.lineTo(p.x, p.y);
      return *this;
    }

    Path& close() {
      m_skPath.close();
      return *this;
    }

    void offset(float dx, float dy, Path* dst) const {
      m_skPath.offset(dx, dy, &dst->m_skPath);
    }

    void offset(float dx, float dy) {
      m_skPath.offset(dx, dy);
    }

    void transform(const Matrix& matrix, Path* dst) {
      m_skPath.transform(matrix.skMatrix(), &dst->m_skPath);
    }

    void transform(const Matrix& matrix) {
      m_skPath.transform(matrix.skMatrix());
    }

    RectF bounds() const {
      if (isEmpty())
        return RectF();

      SkRect rc = m_skPath.computeTightBounds();
      return RectF(rc.x(), rc.y(), rc.width(), rc.height());
    }

    const SkPath& skPath() const { return m_skPath; }
    SkPath& skPath() { return m_skPath; }

  private:
    SkPath m_skPath;
  };

} // namespace gfx

#endif
