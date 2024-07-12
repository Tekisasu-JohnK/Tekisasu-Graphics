// LAF OS Library
// Copyright (C) 2018-2021  Igara Studio S.A.
// Copyright (C) 2015-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_VIEW_H_INCLUDED
#define OS_OSX_VIEW_H_INCLUDED
#pragma once

#include "os/event.h"
#include "os/pointer_type.h"

#ifdef __OBJC__

#include <Cocoa/Cocoa.h>

namespace os {
  class WindowOSX;
}

@interface ViewOSX : NSView {
@private
  NSTrackingArea* __strong m_trackingArea;
  NSCursor* __strong m_nsCursor;
  bool m_visibleMouse;
  os::PointerType m_pointerType;
  os::WindowOSX* __weak m_impl;
}
- (id)initWithFrame:(NSRect)frameRect;
- (void)dealloc;
- (void)removeImpl;
- (BOOL)acceptsFirstResponder;
- (BOOL)acceptsFirstMouse:(NSEvent*)event;
- (BOOL)mouseDownCanMoveWindow;
- (void)viewDidChangeBackingProperties;
- (void)viewDidHide;
- (void)viewDidUnhide;
- (void)viewDidMoveToWindow;
- (void)drawRect:(NSRect)dirtyRect;
- (void)keyDown:(NSEvent*)event;
- (void)keyUp:(NSEvent*)event;
- (void)flagsChanged:(NSEvent*)event;
+ (void)updateKeyFlags:(NSEvent*)event;
- (void)mouseEntered:(NSEvent*)event;
- (void)mouseMoved:(NSEvent*)event;
- (void)mouseExited:(NSEvent*)event;
- (void)mouseDown:(NSEvent*)event;
- (void)mouseUp:(NSEvent*)event;
- (void)mouseDragged:(NSEvent*)event;
- (void)rightMouseDown:(NSEvent*)event;
- (void)rightMouseUp:(NSEvent*)event;
- (void)rightMouseDragged:(NSEvent*)event;
- (void)otherMouseDown:(NSEvent*)event;
- (void)otherMouseUp:(NSEvent*)event;
- (void)otherMouseDragged:(NSEvent*)event;
- (void)handleMouseDown:(NSEvent*)event;
- (void)handleMouseUp:(NSEvent*)event;
- (void)handleMouseDragged:(NSEvent*)event;
- (void)scrollWheel:(NSEvent*)event;
- (void)cursorUpdate:(NSEvent*)event;
- (void)setCursor:(NSCursor*)cursor;
- (void)setFrameSize:(NSSize)newSize;
- (void)createMouseTrackingArea;
- (void)destroyMouseTrackingArea;
- (void)updateCurrentCursor;
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender;
- (void)doCommandBySelector:(SEL)selector;
- (void)setTranslateDeadKeys:(BOOL)state;
- (void)queueEvent:(os::Event&)ev;
@end

#endif

#endif
