// LAF Gfx Library
// Copyright (c) 2018-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_COLOR_SPACE_H_INCLUDED
#define GFX_COLOR_SPACE_H_INCLUDED
#pragma once

#include "base/ref.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#pragma push_macro("None")
#undef None // Undefine the X11 None macro

namespace gfx {

  class ColorSpace;
  using ColorSpaceRef = base::Ref<ColorSpace>;

  // Gamut white point and primaries as in Skia's SkColorSpacePrimaries.
  struct ColorSpacePrimaries {
    float rx, ry, gx, gy, bx, by; // Red/Green/Blue XY
    float wx, wy;                 // White point XY
  };

  // Transfer coefficients as in Skia's skcms_TransferFunction, to
  // transform from a curved space to linear:
  //
  //   LinearVal = C*InputVal + F        , for 0.0f <= InputVal <  D
  //   LinearVal = (A*InputVal + B)^G + E, for D    <= InputVal <= 1.0f
  //
  struct ColorSpaceTransferFn {
    float g, a, b, c, d, e, f;
  };

  class ColorSpace : public base::RefCountT<ColorSpace> {
  public:
    enum Type { None,
                sRGB,
                RGB,
                ICC };

    enum Flag { NoFlags = 0,
                HasGamma = 1,
                HasTransferFn = 2,
                HasPrimaries = 4,
                HasICC = 8 };

    ColorSpace(const Type type,
               const Flag flags = NoFlags,
               const float gamma = 1.0,
               std::vector<uint8_t>&& rawData = std::vector<uint8_t>());

    static ColorSpaceRef MakeNone();   // Use display color space
    static ColorSpaceRef MakeSRGB();
    static ColorSpaceRef MakeLinearSRGB();
    static ColorSpaceRef MakeSRGBWithGamma(float gamma);
    static ColorSpaceRef MakeRGB(const ColorSpaceTransferFn& fn,
                                 const ColorSpacePrimaries& p);
    static ColorSpaceRef MakeRGBWithSRGBGamut(const ColorSpaceTransferFn& fn);
    static ColorSpaceRef MakeRGBWithSRGBGamma(const ColorSpacePrimaries& p);
    static ColorSpaceRef MakeICC(const void* data, size_t n);
    static ColorSpaceRef MakeICC(std::vector<uint8_t>&& data);

    Type type() const { return m_type; }
    Flag flags() const { return m_flags; }

    const std::string& name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    float gamma() const { return m_gamma; }

    bool hasGamma() const { return has(HasGamma); }
    bool hasTransferFn() const { return has(HasTransferFn); }
    bool hasPrimaries() const { return has(HasPrimaries); }

    size_t iccSize() const {
      if (has(HasICC))
        return m_data.size();
      else
        return 0;
    }

    const void* iccData() const {
      if (has(HasICC))
        return &m_data[0];
      else
        return nullptr;
    }

    const ColorSpaceTransferFn* transferFn() const {
      if (has(HasTransferFn))
        return (const ColorSpaceTransferFn*)&m_data[0];
      else
        return nullptr;
    }

    const ColorSpacePrimaries* primaries() const {
      if (has(HasPrimaries)) {
        if (has(HasTransferFn))
          return (const ColorSpacePrimaries*)&m_data[sizeof(ColorSpaceTransferFn)];
        else
          return (const ColorSpacePrimaries*)&m_data[0];
      }
      else
        return nullptr;
    }

    bool operator==(const ColorSpace& that) const = delete;
    bool operator!=(const ColorSpace& that) const = delete;
    bool nearlyEqual(const ColorSpace& that) const;

    // This data can be used
    const std::vector<uint8_t>& rawData() const { return m_data; }

  private:
    bool has(const Flag flag) const { return (m_flags & int(flag)) == int(flag); }

    Type m_type;
    Flag m_flags;
    std::string m_name;
    float m_gamma = 1.0f;
    // ColorSpacePrimaries + ColorSpaceTransferFn or raw ICC profile data
    std::vector<uint8_t> m_data;
  };

} // namespace gfx

#pragma pop_macro("None")

#endif
