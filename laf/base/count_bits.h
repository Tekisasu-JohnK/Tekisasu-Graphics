// LAF Base Library
// Copyright (c) 2023 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_COUNT_BITS_H_INCLUDED
#define BASE_COUNT_BITS_H_INCLUDED
#pragma once

#include <cstddef>
#include <limits>

namespace base {

  template<typename T>
  constexpr inline size_t count_bits(const T v) {
    size_t n = 0;
    for (size_t b=0; b<sizeof(T)*8; ++b) {
      if (v & (T(1) << b))
        ++n;
    }
    return n;
  }

}

#endif
