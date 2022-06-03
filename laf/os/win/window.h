// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_WINDOW_H_INCLUDED
#define OS_WIN_WINDOW_H_INCLUDED
#pragma once

#include "base/time.h"
#include "gfx/size.h"
#include "os/color_space.h"
#include "os/event.h"
#include "os/native_cursor.h"
#include "os/pointer_type.h"
#include "os/win/wintab.h"

#include <string>
#include <windows.h>
#include <interactioncontext.h>

#define OS_USE_POINTER_API_FOR_MOUSE 0

namespace os {
  class Surface;
  class WindowSystem;

  class WinWindow {
  public:
    WinWindow(int width, int height, int scale);
    ~WinWindow();

    void queueEvent(Event& ev);
    os::ColorSpacePtr colorSpace() const;
    int scale() const { return m_scale; }
    void setScale(int scale);
    void setVisible(bool visible);
    void maximize();
    bool isMaximized() const;
    bool isMinimized() const;
    gfx::Size clientSize() const;
    gfx::Size restoredSize() const;
    void setTitle(const std::string& title);
    void captureMouse();
    void releaseMouse();
    void setMousePosition(const gfx::Point& position);
    bool setNativeMouseCursor(NativeCursor cursor);
    bool setNativeMouseCursor(const os::Surface* surface,
                              const gfx::Point& focus,
                              const int scale);
    void invalidateRegion(const gfx::Region& rgn);
    std::string getLayout();

    void setLayout(const std::string& layout);
    void setTranslateDeadKeys(bool state);
    void setInterpretOneFingerGestureAsMouseMovement(bool state);
    void onTabletAPIChange();

    HWND handle() { return m_hwnd; }

  private:
    bool setCursor(HCURSOR hcursor, bool custom);
    LRESULT wndProc(UINT msg, WPARAM wparam, LPARAM lparam);
    void mouseEvent(LPARAM lparam, Event& ev);
    bool pointerEvent(WPARAM wparam, Event& ev, POINTER_INFO& pi);
    void handlePointerButtonChange(Event& ev, POINTER_INFO& pi);
    void handleInteractionContextOutput(
      const INTERACTION_CONTEXT_OUTPUT* output);

    void waitTimerToConvertFingerAsMouseMovement();
    void convertFingerAsMouseMovement();
    void delegateFingerToInteractionContext();
    void sendDelayedTouchEvents();
    void clearDelayedTouchEvents();
    void killTouchTimer();
    void checkColorSpaceChange();

    void openWintabCtx();
    void closeWintabCtx();

    virtual void onQueueEvent(Event& ev) { }
    virtual void onResize(const gfx::Size& sz) { }
    virtual void onStartResizing() { }
    virtual void onEndResizing() { }
    virtual void onPaint(HDC hdc) { }
    virtual void onChangeColorSpace() { }

    static void registerClass();
    static HWND createHwnd(WinWindow* self, int width, int height);
    static LRESULT CALLBACK staticWndProc(
      HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static void CALLBACK staticInteractionContextCallback(
      void* clientData,
      const INTERACTION_CONTEXT_OUTPUT* output);

    static WindowSystem* system();

    mutable HWND m_hwnd;
    HCURSOR m_hcursor;
    gfx::Size m_clientSize;
    gfx::Size m_restoredSize;
    int m_scale;
    bool m_isCreated;
    bool m_translateDeadKeys;
    bool m_hasMouse;
    bool m_captureMouse;
    bool m_customHcursor;

    // To change the color profile
    mutable std::string m_lastICCProfile;
    mutable os::ColorSpacePtr m_lastColorProfile;

    // Windows 8 pointer API
    bool m_usePointerApi;
    UINT32 m_lastPointerId;
    UINT32 m_capturePointerId;
    HINTERACTIONCONTEXT m_ictx;

    // This might be the most ugliest hack I've done to fix a Windows
    // bug. Here's the thing:
    // 1) When we use the pen on a Surface device, it send us
    //    WM_POINTERUPDATE messages, but after a WM_POINTERUPDATE
    //    we'll receive a WM_MOUSEMOVE with the position of the mouse
    //    (which is different to the position of the pen). This
    //    happens constantly if we press a modifier key like Alt,
    //    Ctrl, or Shift, and randomly if we don't use a modifier.
    // 2) First I tried to fix this issue disabling the WM_MOUSEMOVE
    //    processing when a pointer device is inside the window (with
    //    WM_POINTERENTER/LEAVE events), but that generated some buggy
    //    behavior (sometimes we stopped processing WM_MOUSEMOVE
    //    messages completely, it looks like a WM_POINTERLEAVE could
    //    be lost in time).
    // 3) Then I tried to fix this using the Windows pointer API for
    //    mouse messages (EnableMouseInPointer(TRUE)) but that API
    //    doesn't work well with Wacom drivers. And I guess it might
    //    depend on the specific driver version.
    // 4) Now I've reverted the pointer API change (we keep using the
    //    WM_MOUSE* messages) and fixed the original issue with this
    //    "m_ignoreRandomMouseEvents" variable. This is a counter that
    //    indicates the number of WM_MOUSEMOVE messages that we must
    //    ignore after a WM_POINTERUPDATE.
    // 5) Each time we receive a WM_POINTERUPDATE this counter is set
    //    to 2. A value that I calculated practically just testing the
    //    pen in a Surface Pro using the Alt modifier and seeing how
    //    many random WM_MOUSEMOVE messages were received.
    // 6) When a WM_MOUSEMOVE is received this counter is decreased,
    //    so when it backs to 0 we start processing WM_MOUSEMOVE
    //    messages again.
    int m_ignoreRandomMouseEvents;

    // Variables used to convert one finger in mouse-like movement,
    // and two/more fingers in scroll movement/pan/zoom. The idea
    // is as follows:
    // 1) When a PT_TOUCH is received, we count this event in
    //    m_fingers and setup a timer (m_fingerTimerID) to wait for
    //    another touch event.
    // 2) If another touch event is received, we process the messages
    //    with the interaction context (m_ictx) to handle special
    //    gestures (pan, magnify, etc.).
    // 3) If the timeout is reached, and we've received only one
    //    finger on the windows, we can send all awaiting events
    //    (m_fingerEvents) like mouse movement messages/button
    //    presses/releases.
    struct Touch {
      int fingers;              // Number of fingers in the window
      // True when the timeout wasn't reached yet and the finger can be
      // converted to mouse events yet.
      bool canBeMouse;
      // True if we're already processing finger/touch events as mouse
      // movement events.
      bool asMouse;
      // Timeout (WM_TIMER) when the finger is converted to mouse events.
      UINT_PTR timerID;
      // Queued events to be sent when the finger is converted to mouse
      // events (these events are discarded if another finger is
      // introduced in the gesture e.g. to pan)
      std::vector<Event> delayedEvents;
      Touch();
    } *m_touch;

#if OS_USE_POINTER_API_FOR_MOUSE
    // Emulate double-click with pointer API. I guess that this should
    // be done by the Interaction Context API but it looks like
    // messages with pointerType != PT_TOUCH or PT_PEN are just
    // ignored by the ProcessPointerFramesInteractionContext()
    // function even when we call AddPointerInteractionContext() with
    // the given PT_MOUSE pointer.
    bool m_emulateDoubleClick;
    base::tick_t m_doubleClickMsecs;
    base::tick_t m_lastPointerDownTime;
    Event::MouseButton m_lastPointerDownButton;
    int m_pointerDownCount;
#endif

    // Wintab API data
    HCTX m_hpenctx;
    PointerType m_pointerType;
    float m_pressure;
    std::vector<PACKET> m_packets;
    Event m_lastWintabEvent;
  };

} // namespace os

#endif
