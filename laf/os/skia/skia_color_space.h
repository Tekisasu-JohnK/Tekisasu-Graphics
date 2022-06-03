// LAF OS Library
// Copyright (C) 2018  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_COLOR_SPACE_INCLUDED
#define OS_SKIA_SKIA_COLOR_SPACE_INCLUDED
#pragma once

#include "base/disable_copying.h"
#include "os/color_space.h"

#include "SkColorSpace.h"

namespace os {

class SkiaColorSpace : public ColorSpace {
public:
  SkiaColorSpace(const gfx::ColorSpacePtr& gfxcs);

  const gfx::ColorSpacePtr& gfxColorSpace() const override { return m_gfxcs; }
  sk_sp<SkColorSpace> skColorSpace() const { return m_skcs; }

private:
  gfx::ColorSpacePtr m_gfxcs;
  sk_sp<SkColorSpace> m_skcs;

  DISABLE_COPYING(SkiaColorSpace);
};

class SkiaColorSpaceConversion : public ColorSpaceConversion {
public:
  SkiaColorSpaceConversion(const os::ColorSpacePtr& srcColorSpace,
                           const os::ColorSpacePtr& dstColorSpace);

  bool convertRgba(uint32_t* dst, const uint32_t* src, int n) override;
  bool convertGray(uint8_t* dst, const uint8_t* src, int n) override;

private:
  os::ColorSpacePtr m_srcCS;
  os::ColorSpacePtr m_dstCS;
};

} // namespace os

#endif
