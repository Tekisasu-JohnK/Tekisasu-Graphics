// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/debug.h"
#include "os/ref.h"
#include "os/system.h"

namespace os {

static System* g_system = nullptr;

System* make_system_impl();   // Defined on each back-end

SystemRef make_system()
{
  ASSERT(!g_system);
  g_system = make_system_impl();
  return SystemRef(g_system);
}

System* instance()
{
  return g_system;
}

void set_instance(System* system)
{
  g_system = system;
}

} // namespace os
