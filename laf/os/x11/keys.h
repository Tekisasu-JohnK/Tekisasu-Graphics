// LAF OS Library
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_KEYS_INCLUDED
#define OS_X11_KEYS_INCLUDED
#pragma once

#include "gfx/color_space.h"    // Include here avoid error with None
#include "os/keys.h"
#include <X11/X.h>

namespace os {

  extern bool g_spaceBarIsPressed;

  KeyScancode x11_keysym_to_scancode(const KeySym keysym);
  KeySym x11_keysym_to_scancode(const KeyScancode scancode);
  bool x11_is_key_pressed(const KeyScancode scancode);
  int x11_get_unicode_from_scancode(const KeyScancode scancode);

  KeyModifiers get_modifiers_from_x(const int xeventState);

} // namespace os

#endif
