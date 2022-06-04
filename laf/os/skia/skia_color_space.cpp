// LAF OS Library
// Copyright (C) 2018-2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/skia/skia_color_space.h"

#include "base/debug.h"

#include "include/core/SkImageInfo.h"
#include "include/core/SkString.h"
#include "include/third_party/skcms/skcms.h"
#include "src/core/SkConvertPixels.h"

#include <algorithm>

// Defined in skia/src/core/SkICC.cpp
const char* get_color_profile_description(const skcms_TransferFunction& fn,
                                          const skcms_Matrix3x3& toXYZD50);

namespace os {

// Copied from skia/src/core/SkColorSpacePriv.h
static constexpr float gSRGB_toXYZD50[] {
  0.4360747f, 0.3850649f, 0.1430804f, // Rx, Gx, Bx
  0.2225045f, 0.7168786f, 0.0606169f, // Ry, Gy, By
  0.0139322f, 0.0971045f, 0.7141733f, // Rz, Gz, Bz
};

SkiaColorSpace::SkiaColorSpace(const gfx::ColorSpaceRef& gfxcs)
  : m_gfxcs(gfxcs),
    m_skcs(nullptr)
{
  switch (m_gfxcs->type()) {

    case gfx::ColorSpace::None:
      if (m_gfxcs->name().empty())
        m_gfxcs->setName("None");
      break;

    case gfx::ColorSpace::sRGB:
    case gfx::ColorSpace::RGB:
      if (gfxcs->hasGamma()) {
        if (gfxcs->gamma() == 1.0)
          m_skcs = SkColorSpace::MakeSRGBLinear();
        else {
          skcms_TransferFunction fn;
          fn.a = 1.0f;
          fn.b = fn.c = fn.d = fn.e = fn.f = 0.0f;
          fn.g = gfxcs->gamma();
          m_skcs = SkColorSpace::MakeRGB(fn, SkNamedGamut::kSRGB);
        }
      }
      else {
        skcms_TransferFunction skFn;
        skcms_Matrix3x3 toXYZD50;

        if (m_gfxcs->hasPrimaries()) {
          const gfx::ColorSpacePrimaries* primaries = m_gfxcs->primaries();
          if (!skcms_PrimariesToXYZD50(primaries->rx, primaries->ry,
                                       primaries->gx, primaries->gy,
                                       primaries->bx, primaries->by,
                                       primaries->wx, primaries->wy, &toXYZD50)) {
            toXYZD50 = skcms_sRGB_profile()->toXYZD50;
          }
        }

        if (m_gfxcs->hasTransferFn()) {
          const gfx::ColorSpaceTransferFn* fn = m_gfxcs->transferFn();
          skFn.g = fn->g;
          skFn.a = fn->a;
          skFn.b = fn->b;
          skFn.c = fn->c;
          skFn.d = fn->d;
          skFn.e = fn->e;
          skFn.f = fn->f;
        }

        if (m_gfxcs->hasTransferFn()) {
          if (!m_gfxcs->hasPrimaries()) {
            toXYZD50 = skcms_sRGB_profile()->toXYZD50;
          }
          m_skcs = SkColorSpace::MakeRGB(skFn, toXYZD50);
        }
        else if (m_gfxcs->hasPrimaries()) {
          m_skcs = SkColorSpace::MakeRGB(SkNamedTransferFn::kSRGB, toXYZD50);
        }
        else {
          m_skcs = SkColorSpace::MakeSRGB();
        }
      }
      break;

    case gfx::ColorSpace::ICC: {
      skcms_ICCProfile icc;
      if (skcms_Parse(m_gfxcs->iccData(),
                      m_gfxcs->iccSize(), &icc)) {
        m_skcs = SkColorSpace::Make(icc);
      }
      break;
    }
  }

  // TODO read color profile name from ICC data

  if (m_skcs && m_gfxcs->name().empty()) {
    skcms_TransferFunction fn;
    skcms_Matrix3x3 toXYZD50;
    if (m_skcs->isNumericalTransferFn(&fn) &&
        m_skcs->toXYZD50(&toXYZD50)) {
      const char* desc = get_color_profile_description(fn, toXYZD50);
      if (desc)
        m_gfxcs->setName(desc);
    }
  }

  if (m_gfxcs->name().empty())
    m_gfxcs->setName("Custom Profile");
}

SkiaColorSpaceConversion::SkiaColorSpaceConversion(
  const os::ColorSpaceRef& srcColorSpace,
  const os::ColorSpaceRef& dstColorSpace)
  : m_srcCS(srcColorSpace),
    m_dstCS(dstColorSpace)
{
  ASSERT(srcColorSpace);
  ASSERT(dstColorSpace);
}

bool SkiaColorSpaceConversion::convertRgba(uint32_t* dst, const uint32_t* src, int n)
{
  auto dstInfo = SkImageInfo::Make(n, 1, kRGBA_8888_SkColorType, kUnpremul_SkAlphaType,
                                   static_cast<const SkiaColorSpace*>(m_dstCS.get())->skColorSpace());
  auto srcInfo = SkImageInfo::Make(n, 1, kRGBA_8888_SkColorType, kUnpremul_SkAlphaType,
                                   static_cast<const SkiaColorSpace*>(m_srcCS.get())->skColorSpace());
  return SkConvertPixels(dstInfo, dst, 4 * n, srcInfo, src, 4 * n);
}

bool SkiaColorSpaceConversion::convertGray(uint8_t* dst, const uint8_t* src, int n)
{
  auto dstInfo = SkImageInfo::Make(n, 1, kGray_8_SkColorType, kOpaque_SkAlphaType,
                                   static_cast<const SkiaColorSpace*>(m_dstCS.get())->skColorSpace());
  auto srcInfo = SkImageInfo::Make(n, 1, kGray_8_SkColorType, kOpaque_SkAlphaType,
                                   static_cast<const SkiaColorSpace*>(m_srcCS.get())->skColorSpace());
  return SkConvertPixels(dstInfo, dst, n, srcInfo, src, n);
}

} // namespace os
