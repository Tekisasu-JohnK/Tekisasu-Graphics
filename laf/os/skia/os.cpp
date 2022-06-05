// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/skia/skia_system.h"

namespace os {

System* make_system_impl() {
  return new SkiaSystem;
}

void error_message(const char* msg)
{
  fputs(msg, stderr);
  // TODO
}

} // namespace os
