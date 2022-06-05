// LAF OS Library
// Copyright (C) 2020-2021  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <Cocoa/Cocoa.h>
#include <vector>

#include "base/fs.h"
#include "os/common/file_dialog.h"
#include "os/keys.h"
#include "os/native_cursor.h"
#include "os/osx/native_dialogs.h"
#include "os/window.h"

@interface OpenSaveHelper : NSObject {
@private
  NSSavePanel* panel;
  os::Window* window;
  int result;
}
- (id)init;
- (void)setPanel:(NSSavePanel*)panel;
- (void)setWindow:(os::Window*)window;
- (void)runModal;
- (int)result;
@end

@implementation OpenSaveHelper

- (id)init
{
  if (self = [super init]) {
    result = NSFileHandlingPanelCancelButton;
  }
  return self;
}

- (void)setPanel:(NSSavePanel*)newPanel
{
  panel = newPanel;
}

- (void)setWindow:(os::Window*)newWindow
{
  window = newWindow;
}

// This is executed in the main thread.
- (void)runModal
{
  [[[NSApplication sharedApplication] mainMenu] setAutoenablesItems:NO];
  os::NativeCursor oldCursor = window->nativeCursor();
  window->setCursor(os::NativeCursor::Arrow);

#ifndef __MAC_10_6              // runModalForTypes is deprecated in 10.6
  if ([panel isKindOfClass:[NSOpenPanel class]]) {
    // As we're using OS X 10.4 framework, it looks like runModal
    // doesn't recognize the allowedFileTypes property. So we force it
    // using runModalForTypes: selector.

    result = [(NSOpenPanel*)panel runModalForTypes:[panel allowedFileTypes]];
  }
  else
#endif
  {
    result = [panel runModal];
  }

  window->setCursor(oldCursor);
  NSWindow* nsWindow = (__bridge NSWindow *)window->nativeHandle();
  [nsWindow makeKeyAndOrderFront:nil];
  [[[NSApplication sharedApplication] mainMenu] setAutoenablesItems:YES];
}

- (int)result
{
  return result;
}

@end

namespace os {

class FileDialogOSX : public CommonFileDialog {
public:
  FileDialogOSX() {
  }

  std::string fileName() override {
    return m_filename;
  }

  void getMultipleFileNames(base::paths& output) override {
    output = m_filenames;
  }

  void setFileName(const std::string& filename) override {
    m_filename = filename;
  }

  bool show(Window* window) override {
    bool retValue = false;
    @autoreleasepool {
      NSSavePanel* panel = nil;

      if (m_type == Type::SaveFile) {
        panel = [NSSavePanel new];
      }
      else {
        panel = [NSOpenPanel new];
        [(NSOpenPanel*)panel setAllowsMultipleSelection:(m_type == Type::OpenFiles ? YES: NO)];
        [(NSOpenPanel*)panel setCanChooseFiles:(m_type != Type::OpenFolder ? YES: NO)];
        [(NSOpenPanel*)panel setCanChooseDirectories:(m_type == Type::OpenFolder ? YES: NO)];
      }

      [panel setTitle:[NSString stringWithUTF8String:m_title.c_str()]];
      [panel setCanCreateDirectories:YES];

      if (m_type != Type::OpenFolder && !m_filters.empty()) {
        NSMutableArray* types = [[NSMutableArray alloc] init];
        // The first extension in the array is used as the default one.
        if (!m_defExtension.empty())
          [types addObject:[NSString stringWithUTF8String:m_defExtension.c_str()]];
        for (const auto& filter : m_filters)
          [types addObject:[NSString stringWithUTF8String:filter.first.c_str()]];
        [panel setAllowedFileTypes:types];
        if (m_type == Type::SaveFile)
          [panel setAllowsOtherFileTypes:NO];
      }

      // Always show the extension
      [panel setExtensionHidden:NO];

      std::string defPath = base::get_file_path(m_filename);
      std::string defName = base::get_file_name(m_filename);
      if (!defPath.empty())
        [panel setDirectoryURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:defPath.c_str()]]];
      if (!defName.empty())
        [panel setNameFieldStringValue:[NSString stringWithUTF8String:defName.c_str()]];

      OpenSaveHelper* helper = [OpenSaveHelper new];
      [helper setPanel:panel];
      [helper setWindow:window];
      [helper performSelectorOnMainThread:@selector(runModal) withObject:nil waitUntilDone:YES];

      if ([helper result] == NSFileHandlingPanelOKButton) {
        if (m_type == Type::OpenFiles) {
          for (NSURL* url in [(NSOpenPanel*)panel URLs]) {
            m_filename = [[url path] UTF8String];
            m_filenames.push_back(m_filename);
          }
        }
        else {
          NSURL* url = [panel URL];
          m_filename = [[url path] UTF8String];
          m_filenames.push_back(m_filename);
        }
        retValue = true;
      }
    }
    return retValue;
  }

private:

  std::string m_filename;
  base::paths m_filenames;
};

NativeDialogsOSX::NativeDialogsOSX()
{
}

FileDialogRef NativeDialogsOSX::makeFileDialog()
{
  return make_ref<FileDialogOSX>();
}

} // namespace os
