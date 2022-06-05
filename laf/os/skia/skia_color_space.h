// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_COLOR_SPACE_INCLUDED
#define OS_SKIA_SKIA_COLOR_SPACE_INCLUDED
#pragma once

#include "base/disable_copying.h"
#include "os/color_space.h"

#include "include/core/SkColorSpace.h"

namespace os {

class SkiaColorSpace : public ColorSpace {
public:
  SkiaColorSpace(const gfx::ColorSpaceRef& gfxcs);

  const gfx::ColorSpaceRef& gfxColorSpace() const override { return m_gfxcs; }
  sk_sp<SkColorSpace> skColorSpace() const { return m_skcs; }

  const bool isSRGB() const override { return m_skcs->isSRGB(); }

private:
  gfx::ColorSpaceRef m_gfxcs;
  sk_sp<SkColorSpace> m_skcs;

  DISABLE_COPYING(SkiaColorSpace);
};

class SkiaColorSpaceConversion : public ColorSpaceConversion {
public:
  SkiaColorSpaceConversion(const os::ColorSpaceRef& srcColorSpace,
                           const os::ColorSpaceRef& dstColorSpace);

  bool convertRgba(uint32_t* dst, const uint32_t* src, int n) override;
  bool convertGray(uint8_t* dst, const uint8_t* src, int n) override;

private:
  os::ColorSpaceRef m_srcCS;
  os::ColorSpaceRef m_dstCS;
};

} // namespace os

#endif
