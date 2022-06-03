// Aseprite
// Copyright (C) 2019-2020  Igara Studio S.A.
// Copyright (C) 2001-2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/ui/home_view.h"

#include "app/app.h"
#include "app/app_menus.h"
#include "app/commands/commands.h"
#include "app/commands/params.h"
#include "app/crash/data_recovery.h"
#include "app/i18n/strings.h"
#include "app/ui/data_recovery_view.h"
#include "app/ui/main_window.h"
#include "app/ui/recent_listbox.h"
#include "app/ui/skin/skin_theme.h"
#include "app/ui/status_bar.h"
#include "app/ui/workspace.h"
#include "app/ui/workspace_tabs.h"
#include "app/ui_context.h"
#include "base/exception.h"
#include "fmt/format.h"
#include "ui/label.h"
#include "ui/resize_event.h"
#include "ui/system.h"
#include "ui/textbox.h"
#include "ui/view.h"
#include "ver/info.h"

#ifdef ENABLE_NEWS
#include "app/ui/news_listbox.h"
#endif

namespace app {

using namespace ui;
using namespace app::skin;

HomeView::HomeView()
  : m_files(new RecentFilesListBox)
  , m_folders(new RecentFoldersListBox)
#ifdef ENABLE_NEWS
  , m_news(new NewsListBox)
#endif
  , m_dataRecovery(App::instance()->dataRecovery())
  , m_dataRecoveryView(nullptr)
{
  newFile()->Click.connect([this]{ onNewFile(); });
  openFile()->Click.connect([this]{ onOpenFile(); });
  if (m_dataRecovery)
    recoverSprites()->Click.connect([this]{ onRecoverSprites(); });
  else
    recoverSprites()->setVisible(false);

  filesView()->attachToView(m_files);
  foldersView()->attachToView(m_folders);
#ifdef ENABLE_NEWS
  newsView()->attachToView(m_news);
#endif

  checkUpdate()->setVisible(false);

  InitTheme.connect(
    [this]{
      SkinTheme* theme = static_cast<SkinTheme*>(this->theme());
      setBgColor(theme->colors.workspace());
      setChildSpacing(8 * guiscale());
    });
  initTheme();
}

HomeView::~HomeView()
{
#ifdef ENABLE_DATA_RECOVERY
  if (m_dataRecoveryView) {
    ASSERT(!m_dataRecoveryView->parent());
    delete m_dataRecoveryView;
  }
#endif
}

void HomeView::dataRecoverySessionsAreReady()
{
#ifdef ENABLE_DATA_RECOVERY
  if (App::instance()->dataRecovery()->hasRecoverySessions()) {
    // We highlight the "Recover Files" options because we came from a crash
    SkinTheme* theme = static_cast<SkinTheme*>(this->theme());
    recoverSprites()->setStyle(theme->styles.workspaceUpdateLink());
    layout();
  }
  if (m_dataRecoveryView) {
    m_dataRecoveryView->refreshListNotification();
  }
#endif
}

std::string HomeView::getTabText()
{
  return Strings::home_view_title();
}

TabIcon HomeView::getTabIcon()
{
  return TabIcon::HOME;
}

bool HomeView::onCloseView(Workspace* workspace, bool quitting)
{
  workspace->removeView(this);
  return true;
}

void HomeView::onAfterRemoveView(Workspace* workspace)
{
  if (m_dataRecoveryView &&
      m_dataRecoveryView->parent()) {
    workspace->removeView(m_dataRecoveryView);
  }
}

void HomeView::onTabPopup(Workspace* workspace)
{
  Menu* menu = AppMenus::instance()->getTabPopupMenu();
  if (!menu)
    return;

  menu->showPopup(ui::get_mouse_position());
}

void HomeView::onWorkspaceViewSelected()
{
  StatusBar::instance()->showDefaultText();
}

void HomeView::onNewFile()
{
  Command* command = Commands::instance()->byId(CommandId::NewFile());
  UIContext::instance()->executeCommandFromMenuOrShortcut(command);
}

void HomeView::onOpenFile()
{
  Command* command = Commands::instance()->byId(CommandId::OpenFile());
  UIContext::instance()->executeCommandFromMenuOrShortcut(command);
}

void HomeView::onResize(ui::ResizeEvent& ev)
{
  headerPlaceholder()->setVisible(ev.bounds().h > 200*ui::guiscale());
  foldersPlaceholder()->setVisible(ev.bounds().h > 150*ui::guiscale());
#ifdef ENABLE_NEWS
  newsPlaceholder()->setVisible(ev.bounds().w > 200*ui::guiscale());
#else
  newsPlaceholder()->setVisible(false);
#endif

  ui::VBox::onResize(ev);
}

#ifdef ENABLE_UPDATER

void HomeView::onCheckingUpdates()
{
  checkUpdate()->setText(Strings::home_view_checking_updates());
  checkUpdate()->setVisible(true);

  layout();
}

void HomeView::onUpToDate()
{
  checkUpdate()->setText(
    fmt::format(Strings::home_view_is_up_to_date(), get_app_name()));
  checkUpdate()->setVisible(true);

  layout();
}

void HomeView::onNewUpdate(const std::string& url, const std::string& version)
{
  checkUpdate()->setText(
    fmt::format(Strings::home_view_new_version_available(),
                get_app_name(), version));
  checkUpdate()->setUrl(url);
  checkUpdate()->setVisible(true);
  checkUpdate()->InitTheme.connect(
    [this]{
      SkinTheme* theme = static_cast<SkinTheme*>(this->theme());
      checkUpdate()->setStyle(theme->styles.workspaceUpdateLink());
    });
  checkUpdate()->initTheme();

  layout();
}

#endif // ENABLE_UPDATER

void HomeView::onRecoverSprites()
{
#ifdef ENABLE_DATA_RECOVERY
  ASSERT(m_dataRecovery); // "Recover Files" button is hidden when
                          // data recovery is disabled (m_dataRecovery == nullptr)
  if (!m_dataRecovery)
    return;

  if (!m_dataRecoveryView) {
    m_dataRecoveryView = new DataRecoveryView(m_dataRecovery);

    // Restore the "Recover Files" link style when the
    // DataRecoveryView is empty (so there is no more warning icon on
    // it).
    m_dataRecoveryView->Empty.connect(
      [this]{
        SkinTheme* theme = static_cast<SkinTheme*>(this->theme());
        recoverSprites()->setStyle(theme->styles.workspaceLink());
        layout();
      });
  }

  if (!m_dataRecoveryView->parent())
    App::instance()->workspace()->addView(m_dataRecoveryView);

  App::instance()->mainWindow()->getTabsBar()->selectTab(m_dataRecoveryView);
#endif
}

} // namespace app
