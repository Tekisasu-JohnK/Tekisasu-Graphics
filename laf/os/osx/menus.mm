// LAF OS Library
// Copyright (C) 2019-2022  Igara Studio S.A.
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

@interface NSMenuOSX : NSMenu
- (BOOL)performKeyEquivalent:(NSEvent*)event;
@end

@interface NSMenuItemOSX : NSMenuItem {
@public
  os::Ref<os::MenuItemOSX> original;
}
+ (NSMenuItemOSX*)alloc:(const os::Ref<os::MenuItemOSX>&)original;
- (void)dealloc;
- (void)executeMenuItem:(id)sender;
- (void)validateLafMenuItem;
@end

namespace os {

extern bool g_keyEquivalentUsed;
NSMenuItem* g_standardEditMenuItem = nullptr;

class MenuItemOSX : public MenuItem {
public:
  MenuItemOSX(const MenuItemInfo& info);
  ~MenuItemOSX();
  void setText(const std::string& text) override;
  void setSubmenu(const MenuRef& submenu) override;
  void setEnabled(bool state) override;
  void setChecked(bool state) override;
  void setShortcut(const Shortcut& shortcut) override;
  void setAsStandardEditMenuItem() override;
  NSMenuItem* handle() { return m_handle; }

  // Called by NSMenuItemOSX.executeMenuItem
  void execute();
  void validate();

private:
  void syncTitle();

  NSMenuItem* m_handle;
  MenuRef m_submenu;
  std::function<void()> m_execute;
  std::function<void(os::MenuItem*)> m_validate;
};

class MenuOSX : public Menu {
public:
  MenuOSX();
  ~MenuOSX();
  void addItem(const MenuItemRef& item) override;
  void insertItem(const int index, const MenuItemRef& item) override;
  void removeItem(const MenuItemRef& item) override;
  NSMenu* handle() { return m_handle; }
private:
  NSMenu* m_handle;
};

} // namespace os

@implementation NSMenuOSX
- (BOOL)performKeyEquivalent:(NSEvent*)event
{
  BOOL result = [super performKeyEquivalent:event];
  if (result)
    os::g_keyEquivalentUsed = true;
  return result;
}
@end

@implementation NSMenuItemOSX
+ (NSMenuItemOSX*)alloc:(const os::Ref<os::MenuItemOSX>&)original
{
  NSMenuItemOSX* item = [super alloc];
  item->original = original;
  return item;
}
- (void)dealloc
{
  original = nullptr;
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
  // (EventQueueOSX::getEvent with canWait=true) until we move the
  // mouse (i.e. some kind of event is generated).
  os::Event ev;
  ev.setType(os::Event::Callback);
  ev.setCallback([self]{ original->execute(); });
  os::queue_event(ev);
}
- (void)validateLafMenuItem
{
  if (original)
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
          // message in AppDelegateOSX anyway. So
          // AppDelegateOSX.executeMenuItem: will redirect the message
          // to NSMenuItemOSX.executeMenuItem:
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
        [[NSMenuItemOSX alloc:AddRef(this)]
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
    m_submenu.reset();

  if (m_handle) {
    if (m_handle == g_standardEditMenuItem)
      g_standardEditMenuItem = nullptr;

    if (m_handle.parentItem)
      [m_handle.parentItem.submenu removeItem:m_handle];
  }
}

void MenuItemOSX::setText(const std::string& text)
{
  [m_handle setTitle:[NSString stringWithUTF8String:text.c_str()]];
  syncTitle();
}

void MenuItemOSX::setSubmenu(const MenuRef& submenu)
{
  if (m_submenu) {
    ASSERT(m_handle.submenu == ((MenuOSX*)m_submenu.get())->handle());
    m_submenu.reset();
  }

  m_submenu = submenu;
  if (submenu) {
    [m_handle setSubmenu:((MenuOSX*)submenu.get())->handle()];
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

void MenuItemOSX::setAsStandardEditMenuItem()
{
  g_standardEditMenuItem = m_handle;
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
    [((MenuOSX*)m_submenu.get())->handle() setTitle:m_handle.title];
}

//////////////////////////////////////////////////////////////////////
// os::Menu impl

MenuOSX::MenuOSX()
{
  m_handle = [[NSMenuOSX alloc] init];
}

MenuOSX::~MenuOSX()
{
#if 0 // TODO dispose existent items
  std::vector<NSMenuItem*> items;
  for (int i=0, c=m_handle.itemArray.count; i<c; ++i) {
    items.push_back(m_handle.itemArray[i]);
  }

  for (NSMenuItem* nsItem : items) {
    if ([nsItem isKindOfClass:[NSMenuItemOSX class]]) {
      auto item = (NSMenuItemOSX*)nsItem;
      item->original->dispose();
    }
  }
#endif
}

void MenuOSX::addItem(const MenuItemRef& item)
{
  ASSERT(item);
  [m_handle addItem:((MenuItemOSX*)item.get())->handle()];
}

void MenuOSX::insertItem(const int index, const MenuItemRef& item)
{
  ASSERT(item);
  [m_handle insertItem:((MenuItemOSX*)item.get())->handle()
               atIndex:index];
}

void MenuOSX::removeItem(const MenuItemRef& item)
{
  ASSERT(item);
  [m_handle removeItem:((MenuItemOSX*)item.get())->handle()];
}

//////////////////////////////////////////////////////////////////////
// os::Menus impl

MenusOSX::MenusOSX()
{
}

MenuRef MenusOSX::makeMenu()
{
  return make_ref<MenuOSX>();
}

MenuItemRef MenusOSX::makeMenuItem(const MenuItemInfo& info)
{
  return make_ref<MenuItemOSX>(info);
}

void MenusOSX::setAppMenu(const MenuRef& menu)
{
  if (menu)
    [NSApp setMainMenu:((MenuOSX*)menu.get())->handle()];
  else
    [NSApp setMainMenu:nil];
}

} // namespace os
