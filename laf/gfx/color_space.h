// LAF Gfx Library
// Copyright (C) 2018  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GFX_COLOR_SPACE_H_INCLUDED
#define GFX_COLOR_SPACE_H_INCLUDED
#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace gfx {

  class ColorSpace;
  typedef std::shared_ptr<ColorSpace> ColorSpacePtr;

  // Gamut white point and primaries as in Skia's SkColorSpacePrimaries.
  struct ColorSpacePrimaries {
    float wx, wy;                 // White point XY
    float rx, ry, gx, gy, bx, by; // Red/Green/Blue XY
  };

  // Transfer coefficients as in Skia's SkColorSpaceTransferFn, to
  // transform from a curved space to linear:
  //
  //   LinearVal = C*InputVal + F        , for 0.0f <= InputVal <  D
  //   LinearVal = (A*InputVal + B)^G + E, for D    <= InputVal <= 1.0f
  //
  struct ColorSpaceTransferFn {
    float g, a, b, c, d, e, f;
  };

  class ColorSpace {
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

    static ColorSpacePtr MakeNone();   // Use display color space
    static ColorSpacePtr MakeSRGB();
    static ColorSpacePtr MakeLinearSRGB();
    static ColorSpacePtr MakeSRGBWithGamma(float gamma);
    static ColorSpacePtr MakeRGB(const ColorSpaceTransferFn& fn,
                                 const ColorSpacePrimaries& p);
    static ColorSpacePtr MakeRGBWithSRGBGamut(const ColorSpaceTransferFn& fn);
    static ColorSpacePtr MakeRGBWithSRGBGamma(const ColorSpacePrimaries& p);
    static ColorSpacePtr MakeICC(const void* data, size_t n);
    static ColorSpacePtr MakeICC(std::vector<uint8_t>&& data);

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

#endif
