// LAF OS Library
// Copyright (c) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SAMPLING_H_INCLUDED
#define OS_SAMPLING_H_INCLUDED
#pragma once

#pragma push_macro("None")
#undef None // Undefine the X11 None macro

namespace os {

  // Same as SkSamplingOptions struct
  struct Sampling {
    enum class Filter { Nearest, Linear };
    enum class Mipmap { None, Nearest, Linear };
    struct Cubic {
      float B, C;
      static constexpr Cubic Mitchell() { return { 1/3.0f, 1/3.0f }; }
      static constexpr Cubic CatmullRom() { return { 0.0f, 1/2.0f }; }
    };

    bool useCubic = false;
    Cubic cubic = { 0, 0 };
    Filter filter = Filter::Nearest;
    Mipmap mipmap = Mipmap::None;

    Sampling() = default;
    Sampling(const Sampling&) = default;
    Sampling& operator=(const Sampling&) = default;
    Sampling(Filter f, Mipmap m = Mipmap::None)
      : filter(f), mipmap(m) { }
    Sampling(Cubic c)
      : useCubic(true), cubic(c) { }
  };

} // namespace os

#pragma pop_macro("None")

#endif
