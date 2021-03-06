// Aseprite Document Library
// Copyright (c) 2019 Igara Studio S.A.
// Copyright (c) 2001-2014 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DOC_COLOR_MODE_H_INCLUDED
#define DOC_COLOR_MODE_H_INCLUDED
#pragma once

namespace doc {

  enum class ColorMode {
    RGB,
    GRAYSCALE,
    INDEXED,
    BITMAP,
    TILEMAP,
  };

} // namespace doc

#endif
