// LAF OS Library
// Copyright (C) 2019-2021  Igara Studio S.A.
// Copyright (C) 2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_COMMON_EVENT_QUEUE_WITH_RESIZE_DISPLAY_INCLUDED
#define OS_COMMON_EVENT_QUEUE_WITH_RESIZE_DISPLAY_INCLUDED
#pragma once

#include "base/concurrent_queue.h"
#include "base/time.h"
#include "os/event.h"
#include "os/event_queue.h"

#include <atomic>
#include <queue>

namespace os {

#pragma push_macro("None")
#undef None // Undefine the X11 None macro

// Auxiliary class to handle the ResizeDisplay event in a special way.
// We can use the setResizeDisplayEvent() function in this class to
// change a pending resize event to be sent in a near future (after
// 100ms).  If we receive other setResizeDisplayEvent() calls, only
// one ResizeEvent will be generated (the last one).
//
// This class is useful in platforms like X11 where we cannot know
// when the resize operation has started and finished with information
// from the OS. We only receive a sequence of ConfigureNotify events,
// but we never know when it's the last resize event (we can guess
// that is the last resize event when queueEvent() is called with a
// non-Event::None kind of event).
//
// On Windows, we use this event queue behavior just in case we don't
// receive a WM_EXITSIZEMOVE message (I don't have information if this
// can happen, but the timer might be a good solution just in case we
// don't receive the message).
class EventQueueWithResizeDisplay : public EventQueue {
public:
  // This should be called at the beginning of getEvent() member.
  void checkResizeDisplayEvent(bool& canWait) {
    if (m_resizeEvent.type() != Event::None) {
      canWait = false;
      // Wait 100ms to enqueue the event
      if (base::current_tick() - m_resizeEventTick > 100)
        enqueueResizeDisplayEvent();
    }
  }

  void queueEvent(const Event& ev) override {
    // If we are enqueuing another event and we have a pending
    // ResizeDisplay event, we first enqueue the ResizeDisplay event,
    // because it means that the resize operation is over.
    enqueueResizeDisplayEvent();

    m_events.push(ev);
  }

  // Sets the ResizeDisplay to be sent in the near time (150ms in the
  // future). This is used to avoid sending a lot of ResizeDisplay
  // events while the user is resizing the window.
  //
  // Returns true if this is the first ResizeDisplay event of a
  // sequence of resize events.
  bool setResizeDisplayEvent(const Event& resizeEvent) {
    m_resizeEvent = resizeEvent;
    m_resizeEventTick = base::current_tick();

    bool alreadyHadResizeEvent = m_hasResizeEvent.exchange(true);
    return !alreadyHadResizeEvent;
  }

  void enqueueResizeDisplayEvent() {
    if (m_hasResizeEvent.exchange(false)) {
      m_events.push(m_resizeEvent);
      m_resizeEvent.setType(Event::None);
      m_resizeEventTick = 0;
    }
  }

protected:
  base::concurrent_queue<Event> m_events;

  std::atomic<bool> m_hasResizeEvent = { false };
  Event m_resizeEvent;
  base::tick_t m_resizeEventTick = 0;
};

#pragma pop_macro("None")

} // namespace os

#endif
