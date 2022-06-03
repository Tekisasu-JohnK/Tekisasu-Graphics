// LAF OS Library
// Copyright (C) 2019-2021  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <queue>

#include <windows.h>

#include "os/win/event_queue.h"

namespace os {

void WinEventQueue::getEvent(Event& ev, bool canWait)
{
  MSG msg;

  while (m_events.empty()) {
    BOOL res;

    checkResizeDisplayEvent(canWait);

    if (canWait) {
      res = GetMessage(&msg, nullptr, 0, 0);
    }
    else {
      res = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
    }

    if (res) {
      // Avoid transforming WM_KEYDOWN/UP into WM_DEADCHAR/WM_CHAR
      // messages. Dead keys are converted manually in the
      // WM_KEYDOWN processing on our WinWindow<T> class.
      //
      // From MSDN TranslateMessage() documentation:
      //   "WM_KEYDOWN and WM_KEYUP combinations produce a WM_CHAR
      //   or WM_DEADCHAR message."
      // https://msdn.microsoft.com/en-us/library/windows/desktop/ms644955.aspx
      if (msg.message != WM_KEYDOWN &&
          msg.message != WM_KEYUP) {
        TranslateMessage(&msg);
      }
      DispatchMessage(&msg);
    }
    else if (!canWait)
      break;
  }

  if (!m_events.try_pop(ev)) {
    ev.setType(Event::None);
  }
}

} // namespace os
