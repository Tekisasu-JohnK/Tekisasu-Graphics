// LAF OS Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_OSX_DND_H_INCLUDED
#define OS_OSX_DND_H_INCLUDED
#pragma once

#ifdef __OBJC__

#include "base/paths.h"
#include "gfx/point.h"
#include "os/dnd.h"
#include "os/surface.h"

#include <Cocoa/Cocoa.h>

namespace os {
  class DragDataProviderOSX : public DragDataProvider {
  public:
    DragDataProviderOSX(NSPasteboard* pasteboard) : m_pasteboard(pasteboard) {}

  private:
    NSPasteboard* m_pasteboard;

    base::paths getPaths() override;

    SurfaceRef getImage() override;

    std::string getUrl() override;

    bool contains(DragDataItemType type) override;
  };

  NSDragOperation as_nsdragoperation(const os::DropOperation op);
  os::DropOperation as_dropoperation(const NSDragOperation nsdop);
  gfx::Point drag_position(id<NSDraggingInfo> sender);

} // namespace os

#endif

#endif
