// LAF Gfx Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_MATRIX_NONE_H_INCLUDED
#define GFX_MATRIX_NONE_H_INCLUDED
#pragma once

namespace gfx {

  class Matrix {
  public:
    Matrix() { }

    static Matrix MakeScale(float sx, float sy) { return Matrix(); }
    static Matrix MakeScale(float scale) { return Matrix(); }
    static Matrix MakeTrans(float x, float y) { return Matrix(); }
    static Matrix MakeAll(float scaleX, float skewX,  float transX,
                          float skewY,  float scaleY, float transY,
                          float pers0, float pers1, float pers2) {
      return Matrix();
    }

    Matrix& reset() { return *this; }
    bool isIdentity() const { return true; }
    bool isScaleTranslate() const { return false; }
    bool isTranslate() const { return false; }

    float getScaleX() const { return 1.0f; }
    float getScaleY() const { return 1.0f; }
    float getSkewY() const { return 0.0f; }
    float getSkewX() const { return 0.0f; }
    float getTranslateX() const { return 0.0f; }
    float getTranslateY() const { return 0.0f; }
    float getPerspX() const { return 0.0f; }
    float getPerspY() const { return 0.0f; }

    Matrix& setIdentity() { return *this; }
    Matrix& setTranslate(float dx, float dy) { return *this; }
    void setScale(float sx, float sy, float px, float py) { }
    void setScale(float sx, float sy) { }
    void setRotate(float degrees, float px, float py) { }
    void setRotate(float degrees) { }
    void setScaleTranslate(float sx, float sy, float tx, float ty) { }
    Matrix& preTranslate(float dx, float dy) { return *this; }
    Matrix& postTranslate(float dx, float dy) { return *this; }
    RectF mapRect(const RectF& src) const { return RectF(); }
  };

} // namespace gfx

#endif
