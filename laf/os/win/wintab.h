// LAF OS Library
// Copyright (C) 2020-2022  Igara Studio S.A.
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

#include <map>
#include <string>

#include <windows.h>
#include "third_party/wacom/wintab.h"

#define PACKETDATA (PK_CURSOR | PK_BUTTONS | PK_X | PK_Y | PK_NORMAL_PRESSURE)
#define PACKETMODE (PK_BUTTONS)
#include "third_party/wacom/pktdef.h"

namespace os {

  // Wintab API wrapper
  // Read https://developer-docs.wacom.com/intuos-cintiq-business-tablets/docs/wintab-reference for more information.
  class WintabAPI {
  public:
    class Delegate {
    public:
      virtual ~Delegate() { }
      virtual void onWintabID(const std::string& id) = 0;
      virtual void onWintabFields(const std::map<std::string, std::string>& fields) = 0;
    };

    WintabAPI();
    ~WintabAPI();

    void setDelegate(Delegate* delegate);

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

    bool crashedBefore() const { return m_crashedBefore; }
    void resetCrashFileIfPresent();

  private:
    bool loadWintab();
    bool checkDll(std::string& checksum);

    Delegate* m_delegate = nullptr;
    base::dll m_wintabLib = nullptr;
    LONG m_minPressure = 0;
    LONG m_maxPressure = 0;
    int m_queueSize = 1;
    gfx::Rect m_outBounds;

    // Used to avoid calling LoadLibrary() for each created window
    // when the wintab.dll is not found.
    bool m_alreadyTried = false;

    // True if the program crashed when called WTOpen() in a past
    // execution.
    bool m_crashedBefore = false;
  };

} // namespace os

#endif
