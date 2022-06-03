// LAF OS Library
// Copyright (C) 2016-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_EVENT_QUEUE_INCLUDED
#define OS_X11_EVENT_QUEUE_INCLUDED
#pragma once

#include "os/common/event_queue_with_resize_display.h"
#include "os/x11/x11.h"

#include <queue>

namespace os {

class X11EventQueue : public EventQueueWithResizeDisplay {
public:
  void getEvent(Event& ev, bool canWait) override;

private:
  void processX11Event(XEvent& event);
};

typedef X11EventQueue EventQueueImpl;

} // namespace os

#endif
