// Aseprite
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/app.h"
#include "app/color.h"
#include "app/color_utils.h"
#include "app/file_selector.h"
#include "app/script/engine.h"
#include "app/script/luacpp.h"
#include "app/ui/color_button.h"
#include "app/ui/color_shades.h"
#include "app/ui/expr_entry.h"
#include "app/ui/filename_field.h"
#include "base/paths.h"
#include "base/remove_from_container.h"
#include "ui/box.h"
#include "ui/button.h"
#include "ui/combobox.h"
#include "ui/entry.h"
#include "ui/grid.h"
#include "ui/label.h"
#include "ui/manager.h"
#include "ui/separator.h"
#include "ui/slider.h"
#include "ui/window.h"

#include <map>
#include <string>
#include <vector>

#ifdef ENABLE_UI

#define TRACE_DIALOG(...) // TRACEARGS(__VA_ARGS__)

namespace app {
namespace script {

using namespace ui;

namespace {

struct Dialog;
std::vector<Dialog*> all_dialogs;

struct Dialog {
  ui::Window window;
  ui::VBox vbox;
  ui::Grid grid;
  ui::HBox* hbox = nullptr;
  bool autoNewRow = false;
  std::map<std::string, ui::Widget*> dataWidgets;
  std::map<std::string, ui::Widget*> labelWidgets;
  int currentRadioGroup = 0;

  // Used to create a new row when a different kind of widget is added
  // in the dialog.
  ui::WidgetType lastWidgetType = ui::kGenericWidget;

  // Used to keep a reference to the last onclick button pressed, so
  // then Dialog.data returns true for the button that closed the
  // dialog.
  ui::Widget* lastButton = nullptr;

  // Reference used to keep the dialog alive (so it's not garbage
  // collected) when it's visible.
  int showRef = LUA_REFNIL;
  lua_State* L = nullptr;

  Dialog()
    : window(ui::Window::WithTitleBar, "Script"),
      grid(2, false) {
    window.addChild(&grid);
    all_dialogs.push_back(this);
  }

  ~Dialog() {
    base::remove_from_container(all_dialogs, this);
  }

  void unrefShowOnClose() {
    window.Close.connect([this](ui::CloseEvent&){ unrefShow(); });
  }

  // When we show the dialog, we reference it from the registry to
  // keep the dialog alive in case that the user declared it as a
  // "local" variable but called Dialog:show{wait=false}
  void refShow(lua_State* L) {
    if (showRef == LUA_REFNIL) {
      this->L = L;
      lua_pushvalue(L, 1);
      showRef = luaL_ref(L, LUA_REGISTRYINDEX);
    }
  }

  // When the dialog is closed, we unreference it from the registry so
  // now the dialog can be GC'd if there are no other references to it
  // (all references to the dialog itself from callbacks are stored in
  // the same dialog uservalue, so when the dialog+callbacks are not
  // used anymore they are GC'd as a group)
  void unrefShow() {
    if (showRef != LUA_REFNIL) {
      luaL_unref(this->L, LUA_REGISTRYINDEX, showRef);
      showRef = LUA_REFNIL;
      L = nullptr;
    }
  }

  Widget* findDataWidgetById(const char* id) {
    auto it = dataWidgets.find(id);
    if (it != dataWidgets.end())
      return it->second;
    else
      return nullptr;
  }

  void setLabelVisibility(const char* id, bool visible) {
    auto it = labelWidgets.find(id);
    if (it != labelWidgets.end())
      it->second->setVisible(visible);
  }

  void setLabelText(const char* id, const char* text) {
    auto it = labelWidgets.find(id);
    if (it != labelWidgets.end())
      it->second->setText(text);
  }

};

template<typename...Args,
         typename Callback>
void Dialog_connect_signal(lua_State* L,
                           int dlgIdx,
                           obs::signal<void(Args...)>& signal,
                           Callback callback)
{
  auto dlg = get_obj<Dialog>(L, dlgIdx);

  // Here we get the uservalue of the dlg (the table with
  // functions/callbacks) and store a copy of the given function in
  // the stack (index=-1) in that table.
  lua_getuservalue(L, dlgIdx);
  lua_len(L, -1);
  const int n = 1+lua_tointegerx(L, -1, nullptr);
  lua_pop(L, 1);           // Pop the length of the table
  lua_pushvalue(L, -2);    // Copy the function in stack
  lua_rawseti(L, -2, n);   // Put the copy of the function in the uservalue
  lua_pop(L, 1);           // Pop the uservalue

  signal.connect(
    [=](Args...args) {
      // In case that the dialog is hidden, we cannot access to the
      // global LUA_REGISTRYINDEX to get its reference.
      if (dlg->showRef == LUA_REFNIL)
        return;

      try {
        // Get the function "n" from the uservalue table of the dialog
        lua_rawgeti(L, LUA_REGISTRYINDEX, dlg->showRef);
        lua_getuservalue(L, -1);
        lua_rawgeti(L, -1, n);

        // Use the callback with a special table in the Lua stack to
        // send it as parameter to the Lua function in the
        // lua_pcall() (that table is like an "event data" parameter
        // for the function).
        lua_newtable(L);
        callback(L, std::forward<Args>(args)...);

        if (lua_isfunction(L, -2)) {
          if (lua_pcall(L, 1, 0, 0)) {
            if (const char* s = lua_tostring(L, -1))
              App::instance()
                ->scriptEngine()
                ->consolePrint(s);
          }
        }
        else {
          lua_pop(L, 1); // Pop the value which should have been a function
        }
        lua_pop(L, 2);   // Pop uservalue & userdata
      }
      catch (const std::exception& ex) {
        // This is used to catch unhandled exception or for
        // example, std::runtime_error exceptions when a Tx() is
        // created without an active sprite.
        App::instance()
          ->scriptEngine()
          ->consolePrint(ex.what());
      }
    });
}

int Dialog_new(lua_State* L)
{
  auto dlg = push_new<Dialog>(L);

  // The uservalue of the dialog userdata will contain a table that
  // stores all the callbacks to handle events. As these callbacks can
  // reference the dialog itself, it's important to store callbacks in
  // this table that depends on the dialog lifetime itself
  // (i.e. uservalue) and in the global registry, because in that case
  // we could create a cyclic reference that would be not GC'd.
  lua_newtable(L);
  lua_setuservalue(L, -2);

  if (lua_isstring(L, 1)) {
    dlg->window.setText(lua_tostring(L, 1));
  }
  else if (lua_istable(L, 1)) {
    int type = lua_getfield(L, 1, "title");
    if (type != LUA_TNIL)
      dlg->window.setText(lua_tostring(L, -1));
    lua_pop(L, 1);

    type = lua_getfield(L, 1, "onclose");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, -2, dlg->window.Close,
        [](lua_State*, CloseEvent&){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  // The showRef must be the last reference to the dialog to be
  // unreferenced after the window is closed (that's why this is the
  // last connection to ui::Window::Close)
  dlg->unrefShowOnClose();

  TRACE_DIALOG("Dialog_new", dlg);
  return 1;
}

int Dialog_gc(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  TRACE_DIALOG("Dialog_gc", dlg);
  dlg->~Dialog();
  return 0;
}

int Dialog_show(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  dlg->refShow(L);

  bool wait = true;
  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "wait");
    if (type == LUA_TBOOLEAN)
      wait = lua_toboolean(L, -1);
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "bounds");
    if (VALID_LUATYPE(type)) {
      const auto rc = convert_args_into_rect(L, -1);
      if (!rc.isEmpty()) {
        dlg->window.remapWindow();
        dlg->window.setBounds(rc);
      }
    }
    lua_pop(L, 1);
  }

  if (wait)
    dlg->window.openWindowInForeground();
  else
    dlg->window.openWindow();

  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_close(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  dlg->window.closeWindow(nullptr);
  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_add_widget(lua_State* L, Widget* widget)
{
  auto dlg = get_obj<Dialog>(L, 1);
  const char* label = nullptr;
  std::string id;
  bool visible = true;

  // This is to separate different kind of widgets without label in
  // different rows.
  if (dlg->lastWidgetType != widget->type() ||
      dlg->autoNewRow) {
    dlg->lastWidgetType = widget->type();
    dlg->hbox = nullptr;
  }

  if (lua_istable(L, 2)) {
    // Widget ID (used to fill the Dialog_get_data table then)
    int type = lua_getfield(L, 2, "id");
    if (type == LUA_TSTRING) {
      if (auto s = lua_tostring(L, -1)) {
        id = s;
        widget->setId(s);
        dlg->dataWidgets[id] = widget;
      }
    }
    lua_pop(L, 1);

    // Label
    type = lua_getfield(L, 2, "label");
    if (type != LUA_TNIL)
      label = lua_tostring(L, -1);
    lua_pop(L, 1);

    // Focus magnet
    type = lua_getfield(L, 2, "focus");
    if (type != LUA_TNIL && lua_toboolean(L, -1))
      widget->setFocusMagnet(true);
    lua_pop(L, 1);

    // Enabled
    type = lua_getfield(L, 2, "enabled");
    if (type != LUA_TNIL)
      widget->setEnabled(lua_toboolean(L, -1));
    lua_pop(L, 1);

    // Visible
    type = lua_getfield(L, 2, "visible");
    if (type != LUA_TNIL) {
      visible = lua_toboolean(L, -1);
      widget->setVisible(visible);
    }
    lua_pop(L, 1);
  }

  if (label || !dlg->hbox) {
    if (label) {
      auto labelWidget = new ui::Label(label);
      if (!visible)
        labelWidget->setVisible(false);

      dlg->grid.addChildInCell(labelWidget, 1, 1, ui::LEFT | ui::TOP);
      if (!id.empty())
        dlg->labelWidgets[id] = labelWidget;
    }
    else
      dlg->grid.addChildInCell(new ui::HBox, 1, 1, ui::LEFT | ui::TOP);

    auto hbox = new ui::HBox;
    if (widget->type() == ui::kButtonWidget)
      hbox->enableFlags(ui::HOMOGENEOUS);
    dlg->grid.addChildInCell(hbox, 1, 1, ui::HORIZONTAL | ui::TOP);
    dlg->hbox = hbox;
  }

  widget->setExpansive(true);
  dlg->hbox->addChild(widget);

  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_newrow(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  dlg->hbox = nullptr;

  dlg->autoNewRow = false;
  if (lua_istable(L, 2)) {
    // Dialog:newrow{ always }
    if (lua_is_key_true(L, 2, "always"))
      dlg->autoNewRow = true;
    lua_pop(L, 1);
  }

  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_separator(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);

  std::string id, text;

  if (lua_isstring(L, 2)) {
    if (auto p = lua_tostring(L, 2))
      text = p;
  }
  else if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "text");
    if (type == LUA_TSTRING) {
      if (auto p = lua_tostring(L, -1))
        text = p;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "id");
    if (type == LUA_TSTRING) {
      if (auto s = lua_tostring(L, -1))
        id = s;
    }
    lua_pop(L, 1);
  }

  auto widget = new ui::Separator(text, ui::HORIZONTAL);
  if (!id.empty()) {
    widget->setId(id.c_str());
    dlg->dataWidgets[id] = widget;
  }

  dlg->grid.addChildInCell(widget, 2, 1, ui::HORIZONTAL | ui::TOP);
  dlg->hbox = nullptr;

  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_label(lua_State* L)
{
  std::string text;
  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "text");
    if (type != LUA_TNIL) {
      if (auto p = lua_tostring(L, -1))
        text = p;
    }
    lua_pop(L, 1);
  }

  auto widget = new ui::Label(text.c_str());
  return Dialog_add_widget(L, widget);
}

template<typename T>
int Dialog_button_base(lua_State* L, T** outputWidget = nullptr)
{
  std::string text;
  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "text");
    if (type != LUA_TNIL) {
      if (auto p = lua_tostring(L, -1))
        text = p;
    }
    lua_pop(L, 1);
  }

  auto widget = new T(text.c_str());
  if (outputWidget)
    *outputWidget = widget;

  widget->processMnemonicFromText();

  bool closeWindowByDefault = (widget->type() == ui::kButtonWidget);

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "selected");
    if (type != LUA_TNIL)
      widget->setSelected(lua_toboolean(L, -1));
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "onclick");
    if (type == LUA_TFUNCTION) {
      auto dlg = get_obj<Dialog>(L, 1);
      Dialog_connect_signal(
        L, 1, widget->Click,
        [dlg, widget](lua_State* L, Event&){
          if (widget->type() == ui::kButtonWidget)
            dlg->lastButton = widget;
        });
      closeWindowByDefault = false;
    }
    lua_pop(L, 1);
  }

  if (closeWindowByDefault)
    widget->Click.connect([widget](ui::Event&){ widget->closeWindow(); });

  return Dialog_add_widget(L, widget);
}

int Dialog_button(lua_State* L)
{
  return Dialog_button_base<ui::Button>(L);
}

int Dialog_check(lua_State* L)
{
  return Dialog_button_base<ui::CheckBox>(L);
}

int Dialog_radio(lua_State* L)
{
  ui::RadioButton* radio = nullptr;
  const int res = Dialog_button_base<ui::RadioButton>(L, &radio);
  if (radio) {
    auto dlg = get_obj<Dialog>(L, 1);
    bool hasLabelField = false;

    if (lua_istable(L, 2)) {
      int type = lua_getfield(L, 2, "label");
      if (type == LUA_TSTRING)
        hasLabelField = true;
      lua_pop(L, 1);
    }

    if (dlg->currentRadioGroup == 0 ||
        hasLabelField) {
      ++dlg->currentRadioGroup;
    }

    radio->setRadioGroup(dlg->currentRadioGroup);
  }
  return res;
}

int Dialog_entry(lua_State* L)
{
  std::string text;
  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "text");
    if (type == LUA_TSTRING) {
      if (auto p = lua_tostring(L, -1))
        text = p;
    }
    lua_pop(L, 1);
  }

  auto widget = new ui::Entry(4096, text.c_str());

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_number(lua_State* L)
{
  auto widget = new ExprEntry;

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "text");
    if (type == LUA_TSTRING) {
      if (auto p = lua_tostring(L, -1))
        widget->setText(p);
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "decimals");
    if (type != LUA_TNIL) {
      widget->setDecimals(lua_tointegerx(L, -1, nullptr));
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_slider(lua_State* L)
{
  int min = 0;
  int max = 100;
  int value = 100;

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "min");
    if (type != LUA_TNIL) {
      min = lua_tointegerx(L, -1, nullptr);
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "max");
    if (type != LUA_TNIL) {
      max = lua_tointegerx(L, -1, nullptr);
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "value");
    if (type != LUA_TNIL) {
      value = lua_tointegerx(L, -1, nullptr);
    }
    lua_pop(L, 1);
  }

  auto widget = new ui::Slider(min, max, value);

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "onrelease");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->SliderReleased,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_combobox(lua_State* L)
{
  auto widget = new ui::ComboBox;

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "options");
    if (type == LUA_TTABLE) {
      lua_pushnil(L);
      while (lua_next(L, -2) != 0) {
        if (auto p = lua_tostring(L, -1))
          widget->addItem(p);
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "option");
    if (type == LUA_TSTRING) {
      if (auto p = lua_tostring(L, -1)) {
        int index = widget->findItemIndex(p);
        if (index >= 0)
          widget->setSelectedItemIndex(index);
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_color(lua_State* L)
{
  app::Color color;
  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "color");
    color = convert_args_into_color(L, -1);
    lua_pop(L, 1);
  }

  auto widget = new ColorButton(color,
                                app_get_current_pixel_format(),
                                ColorButtonOptions());

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L, const app::Color& color){
          push_obj<app::Color>(L, color);
          lua_setfield(L, -2, "color");
        });
    }
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_shades(lua_State* L)
{
  Shade colors;
  // 'pick' is the default mode anyway
  ColorShades::ClickType mode = ColorShades::ClickEntries;

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "mode");
    if (type == LUA_TSTRING) {
      if (const char* modeStr = lua_tostring(L, -1)) {
        if (base::utf8_icmp(modeStr, "pick") == 0)
          mode = ColorShades::ClickEntries;
        else if (base::utf8_icmp(modeStr, "sort") == 0)
          mode = ColorShades::DragAndDropEntries;
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "colors");
    if (type == LUA_TTABLE) {
      lua_pushnil(L);
      while (lua_next(L, -2) != 0) {
        app::Color color = convert_args_into_color(L, -1);
        colors.push_back(color);
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);
  }

  auto widget = new ColorShades(colors, mode);

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "onclick");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Click,
        [widget](lua_State* L, ColorShades::ClickEvent& ev){
          lua_pushinteger(L, (int)ev.button());
          lua_setfield(L, -2, "button");

          const int i = widget->getHotEntry();
          const Shade shade = widget->getShade();
          if (i >= 0 && i < int(shade.size())) {
            push_obj<app::Color>(L, shade[i]);
            lua_setfield(L, -2, "color");
          }
        });
    }
    lua_pop(L, 1);
  }

  return Dialog_add_widget(L, widget);
}

int Dialog_file(lua_State* L)
{
  std::string title = "Open File";
  std::string fn;
  base::paths exts;
  auto dlgType = FileSelectorType::Open;
  auto fnFieldType = FilenameField::ButtonOnly;

  if (lua_istable(L, 2)) {
    lua_getfield(L, 2, "filename");
    if (auto p = lua_tostring(L, -1))
      fn = p;
    lua_pop(L, 1);

    int type = lua_getfield(L, 2, "save");
    if (type == LUA_TBOOLEAN && lua_toboolean(L, -1)) {
      dlgType = FileSelectorType::Save;
      title = "Save File";
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "title");
    if (type == LUA_TSTRING)
      title = lua_tostring(L, -1);
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "entry");
    if (type == LUA_TBOOLEAN) {
      fnFieldType = FilenameField::EntryAndButton;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "filetypes");
    if (type == LUA_TTABLE) {
      lua_pushnil(L);
      while (lua_next(L, -2) != 0) {
        if (auto p = lua_tostring(L, -1))
          exts.push_back(p);
        lua_pop(L, 1);
      }
    }
    lua_pop(L, 1);
  }

  auto widget = new FilenameField(fnFieldType, fn);

  if (lua_istable(L, 2)) {
    int type = lua_getfield(L, 2, "onchange");
    if (type == LUA_TFUNCTION) {
      Dialog_connect_signal(
        L, 1, widget->Change,
        [](lua_State* L){
          // Do nothing
        });
    }
    lua_pop(L, 1);
  }

  widget->SelectFile.connect(
    [=]() -> std::string {
      base::paths newfilename;
      if (app::show_file_selector(
            title, widget->filename(), exts,
            dlgType,
            newfilename))
        return newfilename.front();
      else
        return widget->filename();
    });
  return Dialog_add_widget(L, widget);
}

int Dialog_modify(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  if (lua_istable(L, 2)) {
    const char* id = nullptr;
    bool relayout = false;

    int type = lua_getfield(L, 2, "id");
    if (type != LUA_TNIL)
      id = lua_tostring(L, -1);
    lua_pop(L, 1);

    // Modify window itself when no ID is specified
    if (id == nullptr) {
      // "title" or "text" is the same for dialogs
      type = lua_getfield(L, 2, "title");
      if (type == LUA_TNIL) {
        lua_pop(L, 1);
        type = lua_getfield(L, 2, "text");
      }
      if (const char* s = lua_tostring(L, -1)) {
        dlg->window.setText(s);
        relayout = true;
      }
      lua_pop(L, 1);
      return 0;
    }

    // Here we could use dlg->window.findChild(id) but why not use the
    // map directly (it should be faster than iterating over all
    // children).
    Widget* widget = dlg->findDataWidgetById(id);
    if (!widget)
      return luaL_error(L, "Given id=\"%s\" in Dialog:modify{} not found in dialog", id);

    type = lua_getfield(L, 2, "enabled");
    if (type != LUA_TNIL)
      widget->setEnabled(lua_toboolean(L, -1));
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "selected");
    if (type != LUA_TNIL)
      widget->setSelected(lua_toboolean(L, -1));
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "visible");
    if (type != LUA_TNIL) {
      bool state = lua_toboolean(L, -1);
      widget->setVisible(state);
      dlg->setLabelVisibility(id, state);
      relayout = true;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "text");
    if (const char* s = lua_tostring(L, -1)) {
      widget->setText(s);
      relayout = true;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "label");
    if (const char* s = lua_tostring(L, -1)) {
      dlg->setLabelText(id, s);
      relayout = true;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "focus");
    if (type != LUA_TNIL && lua_toboolean(L, -1)) {
      widget->requestFocus();
      relayout = true;
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "decimals");
    if (type != LUA_TNIL) {
      if (auto expr = dynamic_cast<ExprEntry*>(widget)) {
        expr->setDecimals(lua_tointegerx(L, -1, nullptr));
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "min");
    if (type != LUA_TNIL) {
      if (auto slider = dynamic_cast<ui::Slider*>(widget)) {
        slider->setRange(lua_tointegerx(L, -1, nullptr), slider->getMaxValue());
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "max");
    if (type != LUA_TNIL) {
      if (auto slider = dynamic_cast<ui::Slider*>(widget)) {
        slider->setRange(slider->getMinValue(), lua_tointegerx(L, -1, nullptr));
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "value");
    if (type != LUA_TNIL) {
      if (auto slider = dynamic_cast<ui::Slider*>(widget)) {
        slider->setValue(lua_tointegerx(L, -1, nullptr));
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "option");
    if (auto p = lua_tostring(L, -1)) {
      if (auto combobox = dynamic_cast<ui::ComboBox*>(widget)) {
        int index = combobox->findItemIndex(p);
        if (index >= 0)
          combobox->setSelectedItemIndex(index);
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "color");
    if (type != LUA_TNIL) {
      if (auto colorButton = dynamic_cast<ColorButton*>(widget)) {
        colorButton->setColor(convert_args_into_color(L, -1));
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "colors");
    if (type != LUA_TNIL) {
      if (auto colorShade = dynamic_cast<ColorShades*>(widget)) {
        Shade shade;
        if (lua_istable(L, -1)) {
          lua_pushnil(L);
          while (lua_next(L, -2) != 0) {
            app::Color color = convert_args_into_color(L, -1);
            shade.push_back(color);
            lua_pop(L, 1);
          }
        }
        colorShade->setShade(shade);
      }
    }
    lua_pop(L, 1);

    type = lua_getfield(L, 2, "filename");
    if (auto p = lua_tostring(L, -1)) {
      if (auto filenameField = dynamic_cast<FilenameField*>(widget)) {
        filenameField->setFilename(p);
      }
    }
    lua_pop(L, 1);

    // TODO combobox options? shades mode? file title / open / save / filetypes? on* events?

    if (relayout) {
      dlg->window.layout();

      gfx::Rect origBounds = dlg->window.bounds();
      gfx::Rect bounds = origBounds;
      bounds.h = dlg->window.sizeHint().h;
      dlg->window.setBounds(bounds);

      dlg->window.manager()->invalidateRect(origBounds);
    }
  }
  lua_pushvalue(L, 1);
  return 1;
}

int Dialog_get_data(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  lua_newtable(L);
  for (const auto& kv : dlg->dataWidgets) {
    const ui::Widget* widget = kv.second;
    switch (widget->type()) {
      case ui::kSeparatorWidget:
        // Do nothing
        continue;
      case ui::kButtonWidget:
      case ui::kCheckWidget:
      case ui::kRadioWidget:
        lua_pushboolean(L, widget->isSelected() ||
                           dlg->window.closer() == widget ||
                           dlg->lastButton == widget);
        break;
      case ui::kEntryWidget:
        if (auto expr = dynamic_cast<const ExprEntry*>(widget)) {
          if (expr->decimals() == 0)
            lua_pushinteger(L, widget->textInt());
          else
            lua_pushnumber(L, widget->textDouble());
        }
        else {
          lua_pushstring(L, widget->text().c_str());
        }
        break;
      case ui::kLabelWidget:
        lua_pushstring(L, widget->text().c_str());
        break;
      case ui::kSliderWidget:
        if (auto slider = dynamic_cast<const ui::Slider*>(widget)) {
          lua_pushinteger(L, slider->getValue());
        }
        break;
      case ui::kComboBoxWidget:
        if (auto combobox = dynamic_cast<const ui::ComboBox*>(widget)) {
          if (auto sel = combobox->getSelectedItem())
            lua_pushstring(L, sel->text().c_str());
          else
            lua_pushnil(L);
        }
        break;
      default:
        if (auto colorButton = dynamic_cast<const ColorButton*>(widget)) {
          push_obj<app::Color>(L, colorButton->getColor());
        }
        else if (auto colorShade = dynamic_cast<const ColorShades*>(widget)) {
          switch (colorShade->clickType()) {

            case ColorShades::ClickEntries: {
              Shade shade = colorShade->getShade();
              int i = colorShade->getHotEntry();
              if (i >= 0 && i < int(shade.size()))
                push_obj<app::Color>(L, shade[i]);
              else
                lua_pushnil(L);
              break;
            }

            case ColorShades::DragAndDropEntries: {
              lua_newtable(L);
              Shade shade = colorShade->getShade();
              for (int i=0; i<int(shade.size()); ++i) {
                push_obj<app::Color>(L, shade[i]);
                lua_rawseti(L, -2, i+1);
              }
              break;
            }

            default:
              lua_pushnil(L);
              break;

          }
        }
        else if (auto filenameField = dynamic_cast<const FilenameField*>(widget)) {
          lua_pushstring(L, filenameField->filename().c_str());
        }
        else {
          lua_pushnil(L);
        }
        break;
    }
    lua_setfield(L, -2, kv.first.c_str());
  }
  return 1;
}

int Dialog_set_data(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  if (!lua_istable(L, 2))
    return 0;
  for (const auto& kv : dlg->dataWidgets) {
    lua_getfield(L, 2, kv.first.c_str());

    ui::Widget* widget = kv.second;
    switch (widget->type()) {
      case ui::kSeparatorWidget:
        // Do nothing
        break;
      case ui::kButtonWidget:
      case ui::kCheckWidget:
      case ui::kRadioWidget:
        widget->setSelected(lua_toboolean(L, -1));
        break;
      case ui::kEntryWidget:
        if (auto expr = dynamic_cast<ExprEntry*>(widget)) {
          if (expr->decimals() == 0)
            expr->setTextf("%d", lua_tointeger(L, -1));
          else
            expr->setTextf("%.*g", expr->decimals(), lua_tonumber(L, -1));
        }
        else if (auto p = lua_tostring(L, -1)) {
          widget->setText(p);
        }
        break;
      case ui::kLabelWidget:
        if (auto p = lua_tostring(L, -1)) {
          widget->setText(p);
        }
        break;
      case ui::kSliderWidget:
        if (auto slider = dynamic_cast<ui::Slider*>(widget)) {
          slider->setValue(lua_tointeger(L, -1));
        }
        break;
      case ui::kComboBoxWidget:
        if (auto combobox = dynamic_cast<ui::ComboBox*>(widget)) {
          if (auto p = lua_tostring(L, -1)) {
            int index = combobox->findItemIndex(p);
            if (index >= 0)
              combobox->setSelectedItemIndex(index);
          }
        }
        break;
      default:
        if (auto colorButton = dynamic_cast<ColorButton*>(widget)) {
          colorButton->setColor(convert_args_into_color(L, -1));
        }
        else if (auto colorShade = dynamic_cast<ColorShades*>(widget)) {
          switch (colorShade->clickType()) {

            case ColorShades::ClickEntries: {
              // TODO change hot entry?
              break;
            }

            case ColorShades::DragAndDropEntries: {
              Shade shade;
              if (lua_istable(L, -1)) {
                lua_pushnil(L);
                while (lua_next(L, -2) != 0) {
                  app::Color color = convert_args_into_color(L, -1);
                  shade.push_back(color);
                  lua_pop(L, 1);
                }
              }
              colorShade->setShade(shade);
              break;
            }

          }
        }
        else if (auto filenameField = dynamic_cast<FilenameField*>(widget)) {
          if (auto p = lua_tostring(L, -1))
            filenameField->setFilename(p);
        }
        break;
    }

    lua_pop(L, 1);
  }
  return 1;
}

int Dialog_get_bounds(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  if (!dlg->window.isVisible())
    dlg->window.remapWindow();
  push_new<gfx::Rect>(L, dlg->window.bounds());
  return 1;
}

int Dialog_set_bounds(lua_State* L)
{
  auto dlg = get_obj<Dialog>(L, 1);
  const auto rc = get_obj<gfx::Rect>(L, 2);
  if (*rc != dlg->window.bounds()) {
    dlg->window.setBounds(*rc);
    dlg->window.invalidate();
  }
  return 0;
}

const luaL_Reg Dialog_methods[] = {
  { "__gc", Dialog_gc },
  { "show", Dialog_show },
  { "close", Dialog_close },
  { "newrow", Dialog_newrow },
  { "separator", Dialog_separator },
  { "label", Dialog_label },
  { "button", Dialog_button },
  { "check", Dialog_check },
  { "radio", Dialog_radio },
  { "entry", Dialog_entry },
  { "number", Dialog_number },
  { "slider", Dialog_slider },
  { "combobox", Dialog_combobox },
  { "color", Dialog_color },
  { "shades", Dialog_shades },
  { "file", Dialog_file },
  { "modify", Dialog_modify },
  { nullptr, nullptr }
};

const Property Dialog_properties[] = {
  { "data", Dialog_get_data, Dialog_set_data },
  { "bounds", Dialog_get_bounds, Dialog_set_bounds },
  { nullptr, nullptr, nullptr }
};

} // anonymous namespace

DEF_MTNAME(Dialog);

void register_dialog_class(lua_State* L)
{
  REG_CLASS(L, Dialog);
  REG_CLASS_NEW(L, Dialog);
  REG_CLASS_PROPERTIES(L, Dialog);
}

// close all opened Dialogs before closing the UI
void close_all_dialogs()
{
  for (Dialog* dlg : all_dialogs) {
    ASSERT(dlg);
    if (dlg)
      dlg->window.closeWindow(nullptr);
  }
}

} // namespace script
} // namespace app

#endif  // ENABLE_UI
