// LAF OS Library
// Copyright (C) 2021-2022  Igara Studio S.A.
// Copyright (C) 2012-2014  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_NATIVE_CURSOR_H_INCLUDED
#define OS_NATIVE_CURSOR_H_INCLUDED
#pragma once

#include "gfx/fwd.h"

namespace os {

  enum class NativeCursor {
    Hidden,
    Arrow,
    Crosshair,
    IBeam,
    Wait,
    Link,
    Help,
    Forbidden,
    Move,
    SizeNS,
    SizeWE,
    SizeN,
    SizeNE,
    SizeE,
    SizeSE,
    SizeS,
    SizeSW,
    SizeW,
    SizeNW,

    Cursors [[maybe_unused]]
  };

} // namespace os

#endif
