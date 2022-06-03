// Aseprite Document Library
// Copyright (C) 2019  Igara Studio S.A.
// Copyright (C) 2001-2015  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef DOC_HANDLE_ANIDIR_H_INCLUDED
#define DOC_HANDLE_ANIDIR_H_INCLUDED
#pragma once

#include "doc/frame.h"

namespace doc {

  class Sprite;
  class Tag;

  frame_t calculate_next_frame(
    const Sprite* sprite,
    frame_t frame,
    frame_t frameDelta,
    const Tag* tag,
    bool& pingPongForward);

} // namespace doc

#endif
