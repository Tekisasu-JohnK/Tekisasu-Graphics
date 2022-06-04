// LAF Gfx Library
// Copyright (c) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gfx/color_space.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace gfx {

ColorSpace::ColorSpace(const Type type,
                       const Flag flags,
                       const float gamma,
                       std::vector<uint8_t>&& data)
  : m_type(type),
    m_flags(flags),
    m_gamma(gamma),
    m_data(std::move(data))
{
}

// static
ColorSpaceRef ColorSpace::MakeNone()
{
  return base::make_ref<ColorSpace>(None);
}

// static
ColorSpaceRef ColorSpace::MakeSRGB()
{
  return base::make_ref<ColorSpace>(sRGB);
}

// static
ColorSpaceRef ColorSpace::MakeLinearSRGB()
{
  return base::make_ref<ColorSpace>(sRGB, HasGamma, 1.0);
}

// static
ColorSpaceRef ColorSpace::MakeSRGBWithGamma(float gamma)
{
  return base::make_ref<ColorSpace>(sRGB, HasGamma, gamma);
}

// static
ColorSpaceRef ColorSpace::MakeRGB(const ColorSpaceTransferFn& fn,
                                  const ColorSpacePrimaries& p)
{
  std::vector<uint8_t> data(sizeof(ColorSpaceTransferFn) + sizeof(ColorSpacePrimaries));
  std::copy(((const uint8_t*)&fn),
            ((const uint8_t*)&fn) + sizeof(ColorSpaceTransferFn),
            data.begin());
  std::copy(((const uint8_t*)&p),
            ((const uint8_t*)&p) + sizeof(ColorSpacePrimaries),
            data.begin() + sizeof(ColorSpaceTransferFn));
  return base::make_ref<ColorSpace>(
    RGB, Flag(HasTransferFn | HasPrimaries), 1.0, std::move(data));
}

// static
ColorSpaceRef ColorSpace::MakeRGBWithSRGBGamut(const ColorSpaceTransferFn& fn)
{
  std::vector<uint8_t> data(sizeof(ColorSpaceTransferFn));
  std::copy(((const uint8_t*)&fn),
            ((const uint8_t*)&fn) + sizeof(ColorSpaceTransferFn),
            data.begin());
  return base::make_ref<ColorSpace>(sRGB, HasTransferFn, 1.0, std::move(data));
}

// static
ColorSpaceRef ColorSpace::MakeRGBWithSRGBGamma(const ColorSpacePrimaries& p)
{
  std::vector<uint8_t> data(sizeof(ColorSpacePrimaries));
  std::copy(((const uint8_t*)&p),
            ((const uint8_t*)&p) + sizeof(ColorSpacePrimaries),
            data.begin());
  return base::make_ref<ColorSpace>(sRGB, HasPrimaries, 1.0, std::move(data));
}

// static
ColorSpaceRef ColorSpace::MakeICC(const void* data, size_t n)
{
  std::vector<uint8_t> newData(n);
  std::copy(((const uint8_t*)data),
            ((const uint8_t*)data)+n, newData.begin());
  return base::make_ref<ColorSpace>(ICC, HasICC, 1.0, std::move(newData));
}

// static
ColorSpaceRef ColorSpace::MakeICC(std::vector<uint8_t>&& data)
{
  return base::make_ref<ColorSpace>(ICC, HasICC, 1.0, std::move(data));
}

// Based on code in skia/src/core/SkICC.cpp by Google Inc.
static bool nearly_equal(float x, float y) {
  // A note on why I chose this tolerance:  transfer_fn_almost_equal() uses a
  // tolerance of 0.001, which doesn't seem to be enough to distinguish
  // between similar transfer functions, for example: gamma2.2 and sRGB.
  //
  // If the tolerance is 0.0f, then this we can't distinguish between two
  // different encodings of what is clearly the same colorspace.  Some
  // experimentation with example files lead to this number:
  static constexpr float kTolerance = 1.0f / (1 << 11);
  return fabsf(x - y) <= kTolerance;
}

static bool nearly_equal(const ColorSpaceTransferFn& u,
                         const ColorSpaceTransferFn& v) {
  return nearly_equal(u.g, v.g)
    && nearly_equal(u.a, v.a)
    && nearly_equal(u.b, v.b)
    && nearly_equal(u.c, v.c)
    && nearly_equal(u.d, v.d)
    && nearly_equal(u.e, v.e)
    && nearly_equal(u.f, v.f);
}

static bool nearly_equal(const ColorSpacePrimaries& u,
                         const ColorSpacePrimaries& v) {
  return nearly_equal(u.wx, v.wx)
    && nearly_equal(u.wy, v.wy)
    && nearly_equal(u.rx, v.ry)
    && nearly_equal(u.ry, v.ry)
    && nearly_equal(u.gx, v.gy)
    && nearly_equal(u.gy, v.gy)
    && nearly_equal(u.bx, v.by)
    && nearly_equal(u.by, v.by);
}

bool ColorSpace::nearlyEqual(const ColorSpace& that) const
{
  if (m_type != that.m_type)
    return false;
  else if (m_type == None)
    return true;
  else if (m_type == sRGB ||
           m_type == RGB) {
    // Gamma
    if (has(HasGamma) && that.has(HasGamma)) {
      if (!nearly_equal(gamma(), that.gamma()))
        return false;
    }
    else if (has(HasGamma) != that.has(HasGamma)) {
      return false;
    }
    // Transfer function
    if (has(HasTransferFn) && that.has(HasTransferFn)) {
      if (!nearly_equal(*transferFn(), *that.transferFn()))
        return false;
    }
    else if (has(HasTransferFn) != that.has(HasTransferFn)) {
      return false;
    }
    // Primaries
    if (has(HasPrimaries) && that.has(HasPrimaries)) {
      if (!nearly_equal(*primaries(), *that.primaries()))
        return false;
    }
    else if (has(HasPrimaries) != that.has(HasPrimaries)) {
      return false;
    }
    return true;
  }
  else if (m_type == ICC) {
    return (m_data == that.m_data);
  }
  return false;
}

} // namespace gfx
