// LAF OS Library
// Copyright (C) 2020-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_XINPUT_INCLUDED
#define OS_X11_XINPUT_INCLUDED
#pragma once

#include "base/dll.h"
#include "os/event.h"
#include "os/x11/keys.h"
#include "os/x11/mouse.h"

#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

#include <map>
#include <vector>

namespace os {

class XInput {
  // To avoid depending on the libXi statically, we can load the
  // libXi.so dynamically.
  typedef XDeviceInfo* (*XListInputDevices_Func)(::Display*, int*);
  typedef void (*XFreeDeviceList_Func)(XDeviceInfo*);
  typedef XDevice* (*XOpenDevice_Func)(::Display*, XID);
  typedef int (*XCloseDevice_Func)(::Display*, XDevice*);
  typedef int (*XSelectExtensionEvent_Func)(::Display*, ::Window, XEventClass*, int);

  XListInputDevices_Func XListInputDevices;
  XFreeDeviceList_Func XFreeDeviceList;
  XOpenDevice_Func XOpenDevice;
  XCloseDevice_Func XCloseDevice;
  XSelectExtensionEvent_Func XSelectExtensionEvent;

public:
  ~XInput();

  void load(::Display* display);
  void unload(::Display* display);

  void selectExtensionEvents(::Display* display, ::Window window);
  bool handleExtensionEvent(const XEvent& xevent);
  void convertExtensionEvent(const XEvent& xevent,
                             Event& ev,
                             int scale,
                             Time& time);

private:
  void addEvent(int type, XEventClass eventClass, Event::Type ourEventype);

  struct Info {
    PointerType pointerType;
    int minPressure = 0;
    int maxPressure = 1000;
  };

  base::dll m_xi = nullptr;
  std::vector<XDevice*> m_openDevices;
  std::map<XID, Info> m_info;
  std::vector<XEventClass> m_eventClasses;
  std::vector<Event::Type> m_eventTypes;
};

} // namespace os

#endif
