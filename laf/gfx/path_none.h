// LAF Gfx Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_PATH_NONE_H_INCLUDED
#define GFX_PATH_NONE_H_INCLUDED
#pragma once

#include "gfx/point.h"
#include "gfx/rect.h"

namespace gfx {

  class Matrix;

  class Path {
  public:
    Path() { }
    Path& reset() { return *this; }
    Path& rewind() { return *this; }
    bool isEmpty() const { return true; }
    Path& moveTo(float x, float y) { return *this; }
    Path& moveTo(const Point& p) { return *this; }
    Path& lineTo(float x, float y) { return *this; }
    Path& lineTo(const Point& p) { return *this; }
    Path& close() { return *this; }
    void offset(float dx, float dy, Path* dst) const { }
    void offset(float dx, float dy) { }
    void transform(const Matrix& matrix, Path* dst) { }
    void transform(const Matrix& matrix) { }
    RectF bounds() const { return RectF(); }
  };

} // namespace gfx

#endif
