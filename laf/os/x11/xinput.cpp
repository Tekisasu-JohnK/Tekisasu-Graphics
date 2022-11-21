// LAF OS Library
// Copyright (C) 2020-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/x11/xinput.h"

#include "base/log.h"
#include "base/string.h"
#include "os/x11/x11.h"

#include <cstring>

#pragma push_macro("None")
#undef None // Undefine the X11 None macro

namespace os {

XInput::~XInput()
{
  if (m_xi) {
    base::unload_dll(m_xi);
    m_xi = nullptr;
  }
}

void XInput::load(::Display* display)
{
  int majorOpcode;
  int firstEvent;
  int firstError;

  // Check that the XInputExtension is available.
  if (!XQueryExtension(display, "XInputExtension",
                       &majorOpcode,
                       &firstEvent,
                       &firstError))
    return;

  m_xi = base::load_dll("libXi.so");
  if (!m_xi) m_xi = base::load_dll("libXi.so.6");
  if (!m_xi) {
    LOG("XI: Error loading libXi.so library\n");
    return;
  }

  XListInputDevices = base::get_dll_proc<XListInputDevices_Func>(m_xi, "XListInputDevices");
  XFreeDeviceList = base::get_dll_proc<XFreeDeviceList_Func>(m_xi, "XFreeDeviceList");
  XOpenDevice = base::get_dll_proc<XOpenDevice_Func>(m_xi, "XOpenDevice");
  XCloseDevice = base::get_dll_proc<XCloseDevice_Func>(m_xi, "XCloseDevice");
  XSelectExtensionEvent = base::get_dll_proc<XSelectExtensionEvent_Func>(m_xi, "XSelectExtensionEvent");

  if (!XListInputDevices ||
      !XFreeDeviceList ||
      !XOpenDevice ||
      !XCloseDevice ||
      !XSelectExtensionEvent) {
    base::unload_dll(m_xi);
    m_xi = nullptr;

    LOG("XI: Error loading functions from libXi.so\n");
    return;
  }

  int ndevices = 0;
  auto devices = XListInputDevices(display, &ndevices);
  if (!devices)
    return;

  std::string userDefinedTablet = X11::instance()->userDefinedTablet();
  if (!userDefinedTablet.empty())
    userDefinedTablet = base::string_to_lower(userDefinedTablet);

  std::string devName;
  for (int i=0; i<ndevices; ++i) {
    XDeviceInfo* devInfo = devices+i;
    if (!devInfo->name)
      continue;

    // Some devices has "stylus" and others "STYLUS".
    devName = base::string_to_lower(devInfo->name);

    PointerType pointerType;
    if (std::strstr(devName.c_str(), "stylus") ||
        // Some devices has "Tablet Pen", others "PenTablet Pen",
        // this case cover both:
        std::strstr(devName.c_str(), "tablet pen") ||
        // Generic driver for stylus with external monitors?
        std::strstr(devName.c_str(), "tablet monitor pen") ||
        // Detect old Wacom Bamboo devices
        std::strstr(devName.c_str(), "wacom bamboo connect pen pen") ||
        // Detect user-defined strings
        (!userDefinedTablet.empty() &&
         std::strstr(devName.c_str(), userDefinedTablet.c_str()))) {
      pointerType = PointerType::Pen;
    }
    // It can be "eraser", or "Tablet Eraser", or "PenTablet
    // Eraser", etc. Anything with "eraser" word should work.
    else if (std::strstr(devName.c_str(), "eraser")) {
      pointerType = PointerType::Eraser;
    }
    else
      continue;

    auto p = (uint8_t*)devInfo->inputclassinfo;
    for (int j=0; j<devInfo->num_classes; ++j, p+=((XAnyClassPtr)p)->length) {
      if (((XAnyClassPtr)p)->c_class != ValuatorClass)
        continue;

      auto valuator = (XValuatorInfoPtr)p;
      // Only for devices with 3 or more axes (axis 0 is X, 1 is Y,
      // and 2 is the pressure).
      if (valuator->num_axes < 3)
        continue;

      Info info;
      info.pointerType = pointerType;
      info.minPressure = valuator->axes[2].min_value;
      info.maxPressure = valuator->axes[2].max_value;

      XDevice* device = XOpenDevice(display, devInfo->id);
      if (!device)
        continue;

      XEventClass eventClass;
      int eventType;

      DeviceButtonPress(device, eventType, eventClass);
      addEvent(eventType, eventClass, Event::MouseDown);

      DeviceButtonRelease(device, eventType, eventClass);
      addEvent(eventType, eventClass, Event::MouseUp);

      DeviceMotionNotify(device, eventType, eventClass);
      addEvent(eventType, eventClass, Event::MouseMove);

      m_info[device->device_id] = info;
      m_openDevices.push_back(device);
    }
  }

  XFreeDeviceList(devices);
}

void XInput::unload(::Display* display)
{
  if (!m_xi)
    return;

  for (XDevice* dev : m_openDevices)
    XCloseDevice(display, dev);
  m_openDevices.clear();
}

void XInput::selectExtensionEvents(::Display* display, ::Window window)
{
  if (!m_xi)
    return;

  ASSERT(XSelectExtensionEvent);
  XSelectExtensionEvent(display, window,
                        m_eventClasses.data(),
                        int(m_eventClasses.size()));
}

bool XInput::handleExtensionEvent(const XEvent& xevent)
{
  return (xevent.type >= 0 &&
          xevent.type < int(m_eventTypes.size()) &&
          m_eventTypes[xevent.type] != Event::None);
}

void XInput::convertExtensionEvent(const XEvent& xevent,
                                   Event& ev,
                                   int scale,
                                   Time& time)
{
  ev.setType(m_eventTypes[xevent.type]);

  gfx::Point pos;
  KeyModifiers modifiers = kKeyNoneModifier;
  Event::MouseButton button = Event::NoneButton;
  XID deviceid;
  int pressure;

  switch (ev.type()) {

    case Event::MouseDown:
    case Event::MouseUp: {
      auto button = (const XDeviceButtonEvent*)&xevent;
      time = button->time;
      deviceid = button->deviceid;
      pos.x = button->x / scale;
      pos.y = button->y / scale;
      modifiers = get_modifiers_from_x(button->state);
      pressure = button->axis_data[2];
      ev.setButton(get_mouse_button_from_x(button->button));
      break;
    }

    case Event::MouseMove: {
      auto motion = (const XDeviceMotionEvent*)&xevent;
      time = motion->time;
      deviceid = motion->deviceid;
      pos.x = motion->x / scale;
      pos.y = motion->y / scale;
      modifiers = get_modifiers_from_x(motion->state);
      pressure = motion->axis_data[2];
      break;
    }

    default:
      ASSERT(false);
      break;
  }

  ev.setModifiers(modifiers);
  ev.setPosition(pos);

  auto it = m_info.find(deviceid);
  ASSERT(it != m_info.end());
  if (it != m_info.end()) {
    const auto& info = it->second;
    if (info.minPressure != info.maxPressure) {
      ev.setPressure(
        float(pressure - info.minPressure) /
        float(info.maxPressure - info.minPressure));
    }
    ev.setPointerType(info.pointerType);
  }
}

void XInput::addEvent(int type, XEventClass eventClass, Event::Type ourEventype)
{
  if (!type || !eventClass)
    return;

  m_eventClasses.push_back(eventClass);

  if (type >= 0 && type < 256) {
    if (type >= m_eventTypes.size())
      m_eventTypes.resize(type+1, Event::None);
    m_eventTypes[type] = ourEventype;
  }
}

} // namespace os

#pragma pop_macro("None")
