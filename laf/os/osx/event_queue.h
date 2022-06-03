// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2015-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_EVENT_QUEUE_INCLUDED
#define OS_OSX_EVENT_QUEUE_INCLUDED
#pragma once

#include "base/concurrent_queue.h"
#include "os/event.h"
#include "os/event_queue.h"

#include <atomic>

namespace os {

class OSXEventQueue : public EventQueue {
public:
  OSXEventQueue();

  void getEvent(Event& ev, bool canWait) override;
  void queueEvent(const Event& ev) override;

private:
  void wakeUpQueue();

  base::concurrent_queue<Event> m_events;
  std::atomic<bool> m_sleeping;
};

typedef OSXEventQueue EventQueueImpl;

} // namespace os

#endif
