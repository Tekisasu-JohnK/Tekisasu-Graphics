// LAF Base Library
// Copyright (c) 2023 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_MASK_SHIFT_H_INCLUDED
#define BASE_MASK_SHIFT_H_INCLUDED
#pragma once

namespace base {

  template<typename T>
  int mask_shift(T mask) {
    int shift = 0;
    int bit = 1;
    while (((mask & bit) == 0) && shift < 8*sizeof(mask)) {
      bit <<= 1;
      ++shift;
    }
    return shift;
  }

}

#endif
