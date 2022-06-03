// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2016-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_WINTAB_H_INCLUDED
#define OS_WIN_WINTAB_H_INCLUDED
#pragma once

#include "base/dll.h"
#include "gfx/rect.h"
#include "os/event.h"

#include <windows.h>
#include "wacom/wintab.h"

#define PACKETDATA (PK_CURSOR | PK_BUTTONS | PK_X | PK_Y | PK_NORMAL_PRESSURE)
#define PACKETMODE (PK_BUTTONS)
#include "wacom/pktdef.h"

namespace os {

  // Wintab API wrapper
  // Read http://www.wacomeng.com/windows/docs/Wintab_v140.htm for more information.
  class WintabAPI {
  public:
    WintabAPI();
    ~WintabAPI();

    HCTX open(HWND hwnd, bool moveMouse);
    void close(HCTX ctx);
    bool packet(HCTX ctx, UINT serial, LPVOID packet);
    int packets(HCTX ctx, int maxPackets, LPVOID packets);
    void overlap(HCTX ctx, BOOL state);

    LONG minPressure() const { return m_minPressure; }
    LONG maxPressure() const { return m_maxPressure; }

    int packetQueueSize() const { return m_queueSize; }
    const gfx::Rect& outBounds() const { return m_outBounds; }

    void mapCursorButton(const int cursor,
                         const int logicalButton,
                         const int relativeButton,
                         Event::Type& evType,
                         Event::MouseButton& mouseButton);

  private:
    bool loadWintab();
    bool checkDll();

    base::dll m_wintabLib;
    LONG m_minPressure = 0;
    LONG m_maxPressure = 0;
    int m_queueSize = 1;
    gfx::Rect m_outBounds;
  };

} // namespace os

#endif
