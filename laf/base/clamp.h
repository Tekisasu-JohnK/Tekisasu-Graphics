// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_CLAMP_H_INCLUDED
#define BASE_CLAMP_H_INCLUDED
#pragma once

namespace base {

  // Differs from std::clamp() because it doesn't use const T& (so
  // constant values are used as references, even Clang with -O2 uses
  // consts/constexprs defined in classes as references).
  template<typename T>
  T clamp(T value, T low, T high) {
    return (value > high ? high:
            (value < low ? low:
                           value));
  }

} // namespace base

#endif
