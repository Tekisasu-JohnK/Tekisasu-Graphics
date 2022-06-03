// LAF OS Library
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_EVENT_QUEUE_INCLUDED
#define OS_WIN_EVENT_QUEUE_INCLUDED
#pragma once

#include "os/common/event_queue_with_resize_display.h"

namespace os {

class WinEventQueue : public EventQueueWithResizeDisplay {
public:
  void getEvent(Event& ev, bool canWait) override;
};

typedef WinEventQueue EventQueueImpl;

} // namespace os

#endif
