// LAF OS Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "base/exception.h"
#include "base/fs.h"
#include "clip/clip.h"
#include "clip/clip_osx.h"
#include "os/dnd.h"
#include "os/osx/dnd.h"
#include "os/osx/window.h"
#include "os/surface_format.h"
#include "os/system.h"

#include <memory>

#ifdef __OBJC__

namespace os {

base::paths DragDataProviderOSX::getPaths()
{
  base::paths files;

  if ([m_pasteboard.types containsObject:NSFilenamesPboardType]) {
    NSArray* filenames = [m_pasteboard propertyListForType:NSFilenamesPboardType];
    for (int i=0; i<[filenames count]; ++i) {
      NSString* fn = [filenames objectAtIndex: i];

      files.push_back(base::normalize_path([fn UTF8String]));
    }
  }
  return files;
}

SurfaceRef DragDataProviderOSX::getImage()
{
  clip::image img;
  clip::image_spec spec;
  if (!clip::osx::get_image_from_clipboard(m_pasteboard, &img, &spec))
    return nullptr;

  return os::instance()->makeSurface(img);
}

std::string DragDataProviderOSX::getUrl()
{
  NSURL* url = [NSURL URLFromPasteboard: m_pasteboard];
  return url ? url.absoluteString.UTF8String : "";
}

bool DragDataProviderOSX::contains(DragDataItemType type)
{
  for (NSPasteboardType t in m_pasteboard.types) {
    if (type == DragDataItemType::Paths &&
        [t isEqual: NSFilenamesPboardType])
      return true;

    if (type == DragDataItemType::Image &&
        ([t isEqual: NSPasteboardTypeTIFF] ||
          [t isEqual: NSPasteboardTypePNG]))
      return true;

    if (type == DragDataItemType::Url &&
        [t isEqual: NSURLPboardType])
      return true;
  }
  return false;
}

NSDragOperation as_nsdragoperation(const os::DropOperation op)
{
  NSDragOperation nsdop = NSDragOperationNone;
  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Copy))
    nsdop |= NSDragOperationCopy;

  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Move))
    nsdop |= NSDragOperationMove;

  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Link))
    nsdop |= NSDragOperationLink;

  return nsdop;
}

os::DropOperation as_dropoperation(const NSDragOperation nsdop)
{
  int op = static_cast<int>(os::DropOperation::None);
  if (nsdop & NSDragOperationCopy)
    op |= static_cast<int>(os::DropOperation::Copy);

  if (nsdop & NSDragOperationMove)
    op |= static_cast<int>(os::DropOperation::Move);

  if (nsdop & NSDragOperationLink)
    op |= static_cast<int>(os::DropOperation::Link);

  return static_cast<os::DropOperation>(op);
}

gfx::Point drag_position(id<NSDraggingInfo> sender)
{
  Window* target = [(WindowOSXObjc*)sender.draggingDestinationWindow impl];
  return target->pointFromScreen(
    gfx::Point(target->contentRect().x + sender.draggingLocation.x,
               target->contentRect().y + target->contentRect().h - sender.draggingLocation.y));
}

} // namespace os

#endif
