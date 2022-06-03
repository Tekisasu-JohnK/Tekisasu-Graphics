// LAF OS Library
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _WIN32
  #include "os/win/event_queue.h"
#elif __APPLE__
  #include "os/osx/event_queue.h"
#else
  #include "os/x11/event_queue.h"
#endif

namespace os {

EventQueueImpl g_queue;

EventQueue* EventQueue::instance() {
  return &g_queue;
}

} // namespace os
