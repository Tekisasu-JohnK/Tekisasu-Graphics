// LAF OS Library
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2012-2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_WINDOW_H_INCLUDED
#define OS_OSX_WINDOW_H_INCLUDED
#pragma once

#include <Cocoa/Cocoa.h>

#include "gfx/point.h"
#include "gfx/rect.h"
#include "gfx/size.h"
#include "os/keys.h"
#include "os/native_cursor.h"

namespace os {
  class Event;
  class Surface;
}

class OSXWindowImpl {
public:
  virtual ~OSXWindowImpl() { }

  void queueEvent(os::Event& ev) {
    onQueueEvent(ev);
  }

  virtual void onQueueEvent(os::Event& ev) = 0;
  virtual void onClose() = 0;
  virtual void onResize(const gfx::Size& size) = 0;
  virtual void onDrawRect(const gfx::Rect& rect) = 0;
  virtual void onWindowChanged() = 0;
  virtual void onStartResizing() = 0;
  virtual void onEndResizing() = 0;

  // This generally happens when the window is moved to another
  // monitor with different scale (e.g. Retina vs non-Retina display),
  // or when the color space changes.
  virtual void onChangeBackingProperties() = 0;
};

@class OSXWindowDelegate;

@interface OSXWindow : NSWindow {
@private
  OSXWindowImpl* m_impl;
  OSXWindowDelegate* m_delegate;
  int m_scale;
}
- (OSXWindow*)initWithImpl:(OSXWindowImpl*)impl
                     width:(int)width
                    height:(int)height
                     scale:(int)scale;
- (OSXWindowImpl*)impl;
- (int)scale;
- (void)setScale:(int)scale;
- (gfx::Size)clientSize;
- (gfx::Size)restoredSize;
- (void)setMousePosition:(const gfx::Point&)position;
- (BOOL)setNativeMouseCursor:(os::NativeCursor)cursor;
- (BOOL)setNativeMouseCursor:(const os::Surface*)surface
                       focus:(const gfx::Point&)focus
                       scale:(const int)scale;
@end

#endif
