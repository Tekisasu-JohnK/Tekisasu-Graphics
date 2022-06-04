// LAF Gfx Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_MATRIX_SKIA_H_INCLUDED
#define GFX_MATRIX_SKIA_H_INCLUDED
#pragma once

#include "gfx/point.h"
#include "gfx/rect.h"

#include "include/core/SkMatrix.h"

namespace gfx {

  // Simple wrapper for SkMatrix
  class Matrix {
  public:
    constexpr Matrix() { }
    constexpr Matrix(const SkMatrix& skMatrix) : m_skMatrix(skMatrix) { }

    static Matrix MakeScale(float sx, float sy) {
      return Matrix(SkMatrix::Scale(sx, sy));
    }

    static Matrix MakeScale(float scale) {
      return Matrix(SkMatrix::Scale(scale, scale));
    }

    static Matrix MakeTrans(float x, float y) {
      return Matrix(SkMatrix::Translate(x, y));
    }

    static Matrix MakeAll(float scaleX, float skewX,  float transX,
                          float skewY,  float scaleY, float transY,
                          float pers0, float pers1, float pers2) {
      return Matrix(SkMatrix::MakeAll(scaleX, skewX,  transX,
                                      skewY,  scaleY, transY,
                                      pers0,  pers1,  pers2));
    }

    Matrix& reset() {
      m_skMatrix.reset();
      return *this;
    }

    bool isIdentity() const { return m_skMatrix.isIdentity(); }
    bool isScaleTranslate() const { return m_skMatrix.isScaleTranslate(); }
    bool isTranslate() const { return m_skMatrix.isTranslate(); }

    float getScaleX() const { return m_skMatrix.getScaleX(); }
    float getScaleY() const { return m_skMatrix.getScaleY(); }
    float getSkewY() const { return m_skMatrix.getSkewY(); }
    float getSkewX() const { return m_skMatrix.getSkewX(); }
    float getTranslateX() const { return m_skMatrix.getTranslateX(); }
    float getTranslateY() const { return m_skMatrix.getTranslateY(); }
    float getPerspX() const { return m_skMatrix.getPerspX(); }
    float getPerspY() const { return m_skMatrix.getPerspY(); }

    Matrix& setIdentity() {
      m_skMatrix.setIdentity();
      return *this;
    }

    Matrix& setTranslate(float dx, float dy) {
      m_skMatrix.setTranslate(dx, dy);
      return *this;
    }

    void setScale(float sx, float sy, float px, float py) {
      m_skMatrix.setScale(sx, sy, px, py);
    }

    void setScale(float sx, float sy) {
      m_skMatrix.setScale(sx, sy);
    }

    void setRotate(float degrees, float px, float py) {
      m_skMatrix.setRotate(degrees, px, py);
    }

    void setRotate(float degrees) {
      m_skMatrix.setRotate(degrees);
    }

    void setScaleTranslate(float sx, float sy, float tx, float ty) {
      m_skMatrix.setScaleTranslate(sx, sy, tx, ty);
    }

    Matrix& preTranslate(float dx, float dy) {
      m_skMatrix.preTranslate(dx, dy);
      return *this;
    }

    Matrix& postTranslate(float dx, float dy) {
      m_skMatrix.postTranslate(dx, dy);
      return *this;
    }

    RectF mapRect(const RectF& src) const {
      SkRect dst;
      m_skMatrix.mapRect(&dst, SkRect::MakeXYWH(SkScalar(src.x), SkScalar(src.y),
                                                SkScalar(src.w), SkScalar(src.h)));
      return RectF(dst.x(), dst.y(), dst.width(), dst.height());
    }

    const SkMatrix& skMatrix() const { return m_skMatrix; }
    SkMatrix& skMatrix() { return m_skMatrix; }

  private:
    SkMatrix m_skMatrix;
  };

} // namespace gfx

#endif
