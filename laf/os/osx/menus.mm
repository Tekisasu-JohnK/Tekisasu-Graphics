// LAF OS Library
// Copyright (C) 2019-2020  Igara Studio S.A.
// Copyright (C) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <Cocoa/Cocoa.h>

#include "base/debug.h"
#include "base/string.h"
#include "os/event.h"
#include "os/event_queue.h"
#include "os/osx/menus.h"
#include "os/shortcut.h"

namespace os {
  class MenuItemOSX;
}

@interface OSXNSMenu : NSMenu
- (BOOL)performKeyEquivalent:(NSEvent*)event;
@end

@interface OSXNSMenuItem : NSMenuItem {
@public
  os::MenuItemOSX* original;
}
+ (OSXNSMenuItem*)alloc:(os::MenuItemOSX*)original;
- (void)executeMenuItem:(id)sender;
- (void)validateMenuItem;
@end

namespace os {

extern bool g_keyEquivalentUsed;

class MenuItemOSX : public MenuItem {
public:
  MenuItemOSX(const MenuItemInfo& info);
  ~MenuItemOSX();
  void dispose() override;
  void setText(const std::string& text) override;
  void setSubmenu(Menu* submenu) override;
  void setEnabled(bool state) override;
  void setChecked(bool state) override;
  void setShortcut(const Shortcut& shortcut) override;
  NSMenuItem* handle() { return m_handle; }

  // Called by OSXNSMenuItem.executeMenuItem
  void execute();
  void validate();

private:
  void syncTitle();
  NSMenuItem* m_handle;
  Menu* m_submenu;
  std::function<void()> m_execute;
  std::function<void(os::MenuItem*)> m_validate;
};

class MenuOSX : public Menu {
public:
  MenuOSX();
  ~MenuOSX();
  void dispose() override;
  void addItem(MenuItem* item) override;
  void insertItem(const int index, MenuItem* item) override;
  void removeItem(MenuItem* item) override;
  NSMenu* handle() { return m_handle; }
private:
  NSMenu* m_handle;
};

} // namespace os

@implementation OSXNSMenu
- (BOOL)performKeyEquivalent:(NSEvent*)event
{
  BOOL result = [super performKeyEquivalent:event];
  if (result)
    os::g_keyEquivalentUsed = true;
  return result;
}
@end

@implementation OSXNSMenuItem
+ (OSXNSMenuItem*)alloc:(os::MenuItemOSX*)original
{
  OSXNSMenuItem* item = [super alloc];
  item->original = original;
  return item;
}
- (void)executeMenuItem:(id)sender
{
  // Execute menu item option in a synchronized way from the events
  // queue processing.
  //
  // Note: It looks like we cannot directly call original->execute()
  // here, because if the callback invalidates some Display region
  // (SkiaWindow::invalidateRegion()) the display is not updated
  // inmediately when the event queue is locked/waiting for events
  // (OSXEventQueue::getEvent with canWait=true) until we move the
  // mouse (i.e. some kind of event is generated).
  os::Event ev;
  ev.setType(os::Event::Callback);
  ev.setCallback([self]{ original->execute(); });
  os::queue_event(ev);
}
- (void)validateMenuItem
{
  original->validate();
}
@end

namespace os {

//////////////////////////////////////////////////////////////////////
// os::MenuItem impl

  MenuItemOSX::MenuItemOSX(const MenuItemInfo& info)
  : m_handle(nullptr)
  , m_submenu(nullptr)
{
  switch (info.type) {

    case MenuItemInfo::Normal: {
      SEL sel = nil;
      id target = nil;
      switch (info.action) {

        case MenuItemInfo::UserDefined:
          sel = @selector(executeMenuItem:);

          // TODO this is strange, it doesn't work, we receive the
          // message in OSXAppDelegate anyway. So
          // OSXAppDelegate.executeMenuItem: will redirect the message
          // to OSXNSMenuItem.executeMenuItem:
          target = m_handle;
          break;

        case MenuItemInfo::Hide:
          sel = @selector(hide:);
          break;

        case MenuItemInfo::HideOthers:
          sel = @selector(hideOtherApplications:);
          break;

        case MenuItemInfo::ShowAll:
          sel = @selector(unhideAllApplications:);
          break;

        case MenuItemInfo::Quit:
          sel = @selector(terminate:);
          break;

        case MenuItemInfo::Minimize:
          sel = @selector(performMiniaturize:);
          break;

        case MenuItemInfo::Zoom:
          sel = @selector(performZoom:);
          break;
      }

      m_handle =
        [[OSXNSMenuItem alloc:this]
            initWithTitle:[NSString stringWithUTF8String:info.text.c_str()]
                   action:sel
            keyEquivalent:@""];

      m_handle.target = target;
      m_execute = info.execute;
      m_validate = info.validate;

      if (!info.shortcut.isEmpty())
        setShortcut(info.shortcut);
      break;
    }

    case MenuItemInfo::Separator:
      m_handle = [NSMenuItem separatorItem];
      break;
  }
}

MenuItemOSX::~MenuItemOSX()
{
  if (m_submenu)
    m_submenu->dispose();

  if (m_handle.parentItem)
    [m_handle.parentItem.submenu removeItem:m_handle];
}

void MenuItemOSX::dispose()
{
  delete this;
}

void MenuItemOSX::setText(const std::string& text)
{
  [m_handle setTitle:[NSString stringWithUTF8String:text.c_str()]];
  syncTitle();
}

void MenuItemOSX::setSubmenu(Menu* submenu)
{
  if (m_submenu) {
    ASSERT(m_handle.submenu == ((MenuOSX*)m_submenu)->handle());
    m_submenu->dispose();
  }

  m_submenu = submenu;
  if (submenu) {
    [m_handle setSubmenu:((MenuOSX*)submenu)->handle()];
    syncTitle();
  }
  else
    [m_handle setSubmenu:nil];
}

void MenuItemOSX::setEnabled(bool state)
{
  m_handle.enabled = state;
}

void MenuItemOSX::setChecked(bool state)
{
  if (state)
    m_handle.state = NSOnState;
  else
    m_handle.state = NSOffState;
}

void MenuItemOSX::setShortcut(const Shortcut& shortcut)
{
  KeyModifiers mods = shortcut.modifiers();
  NSEventModifierFlags nsFlags = 0;
  if (mods & kKeyShiftModifier) nsFlags |= NSEventModifierFlagShift;
  if (mods & kKeyCtrlModifier) nsFlags |= NSEventModifierFlagControl;
  if (mods & kKeyAltModifier) nsFlags |= NSEventModifierFlagOption;
  if (mods & kKeyCmdModifier) nsFlags |= NSEventModifierFlagCommand;

  NSString* keyStr;
  if (shortcut.unicode()) {
    unichar chr = shortcut.unicode();
    keyStr = [NSString stringWithCharacters:&chr length:1];
  }
  else
    keyStr = @"";

  m_handle.keyEquivalent = keyStr;
  m_handle.keyEquivalentModifierMask = nsFlags;
}

void MenuItemOSX::execute()
{
  if (m_execute)
    m_execute();
}

void MenuItemOSX::validate()
{
  if (m_validate)
    m_validate(this);
}

void MenuItemOSX::syncTitle()
{
  // On macOS the submenu title is the one that is displayed in the
  // UI instead of the MenuItem title (so here we copy the menu item
  // title to the submenu title)
  if (m_submenu)
    [((MenuOSX*)m_submenu)->handle() setTitle:m_handle.title];
}

//////////////////////////////////////////////////////////////////////
// os::Menu impl

MenuOSX::MenuOSX()
{
  m_handle = [[OSXNSMenu alloc] init];
}

MenuOSX::~MenuOSX()
{
#if 0 // TODO dispose existent items
  std::vector<NSMenuItem*> items;
  for (int i=0, c=m_handle.itemArray.count; i<c; ++i) {
    items.push_back(m_handle.itemArray[i]);
  }

  for (NSMenuItem* nsItem : items) {
    if ([nsItem isKindOfClass:[OSXNSMenuItem class]]) {
      auto item = (OSXNSMenuItem*)nsItem;
      item->original->dispose();
    }
  }
#endif
}

void MenuOSX::dispose()
{
  delete this;
}

void MenuOSX::addItem(MenuItem* item)
{
  ASSERT(item);
  [m_handle addItem:((MenuItemOSX*)item)->handle()];
}

void MenuOSX::insertItem(const int index, MenuItem* item)
{
  ASSERT(item);
  [m_handle insertItem:((MenuItemOSX*)item)->handle()
               atIndex:index];
}

void MenuOSX::removeItem(MenuItem* item)
{
  ASSERT(item);
  [m_handle removeItem:((MenuItemOSX*)item)->handle()];
}

//////////////////////////////////////////////////////////////////////
// os::Menus impl

MenusOSX::MenusOSX()
{
}

Menu* MenusOSX::createMenu()
{
  return new MenuOSX;
}

MenuItem* MenusOSX::createMenuItem(const MenuItemInfo& info)
{
  return new MenuItemOSX(info);
}

void MenusOSX::setAppMenu(Menu* menu)
{
  if (menu)
    [NSApp setMainMenu:((MenuOSX*)menu)->handle()];
  else
    [NSApp setMainMenu:nil];
}

} // namespace os
