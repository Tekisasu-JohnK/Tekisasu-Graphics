// Aseprite
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2001-2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/script/engine.h"

#include "app/app.h"
#include "app/console.h"
#include "app/doc_exporter.h"
#include "app/doc_range.h"
#include "app/script/luacpp.h"
#include "app/script/security.h"
#include "app/sprite_sheet_type.h"
#include "app/tools/ink_type.h"
#include "base/chrono.h"
#include "base/file_handle.h"
#include "base/fs.h"
#include "base/fstream_path.h"
#include "doc/anidir.h"
#include "doc/blend_mode.h"
#include "doc/color_mode.h"
#include "filters/target.h"
#include "ui/mouse_button.h"

#include <fstream>
#include <sstream>
#include <stack>
#include <string>

namespace app {
namespace script {

namespace {

// High precision clock.
base::Chrono luaClock;

// Stack of script filenames that are being executed.
std::stack<std::string> current_script_dirs;

class AddScriptFilename {
public:
  AddScriptFilename(const std::string& fn) {
    current_script_dirs.push(fn);
  }
  ~AddScriptFilename() {
    current_script_dirs.pop();
  }
};

int print(lua_State* L)
{
  std::string output;
  int n = lua_gettop(L);  /* number of arguments */
  int i;
  lua_getglobal(L, "tostring");
  for (i=1; i<=n; i++) {
    lua_pushvalue(L, -1);  // function to be called
    lua_pushvalue(L, i);   // value to print
    lua_call(L, 1, 1);
    size_t l;
    const char* s = lua_tolstring(L, -1, &l);  // get result
    if (s == nullptr)
      return luaL_error(L, "'tostring' must return a string to 'print'");
    if (i > 1)
      output.push_back('\t');
    output.insert(output.size(), s, l);
    lua_pop(L, 1);  // pop result
  }
  if (!output.empty()) {
    auto app = App::instance();
    if (app && app->scriptEngine())
      app->scriptEngine()->consolePrint(output.c_str());
    else {
      std::printf("%s\n", output.c_str());
      std::fflush(stdout);
    }
  }
  return 0;
}

static int dofilecont(lua_State *L, int d1, lua_KContext d2)
{
  (void)d1;
  (void)d2;
  return lua_gettop(L) - 1;
}

int dofile(lua_State *L)
{
  const char* argFname = luaL_optstring(L, 1, NULL);
  std::string fname = argFname;

  if (!base::is_file(fname) &&
      !current_script_dirs.empty()) {
    // Try to complete a relative filename
    std::string altFname =
      base::join_path(base::get_file_path(current_script_dirs.top()),
                      fname);
    if (base::is_file(altFname))
      fname = altFname;
  }

  lua_settop(L, 1);
  if (luaL_loadfile(L, fname.c_str()) != LUA_OK)
    return lua_error(L);
  {
    AddScriptFilename add(fname);
    lua_callk(L, 0, LUA_MULTRET, 0, dofilecont);
  }
  return dofilecont(L, 0, 0);
}

int os_clock(lua_State* L)
{
  lua_pushnumber(L, luaClock.elapsed());
  return 1;
}

int unsupported(lua_State* L)
{
  // debug.getinfo(1, "n").name
  lua_getglobal(L, "debug");
  lua_getfield(L, -1, "getinfo");
  lua_remove(L, -2);
  lua_pushinteger(L, 1);
  lua_pushstring(L, "n");
  lua_call(L, 2, 1);
  lua_getfield(L, -1, "name");
  return luaL_error(L, "unsupported function '%s'",
                    lua_tostring(L, -1));
}

} // anonymous namespace

void register_app_object(lua_State* L);
void register_app_pixel_color_object(lua_State* L);
void register_app_fs_object(lua_State* L);
void register_app_command_object(lua_State* L);
void register_app_preferences_object(lua_State* L);

void register_brush_class(lua_State* L);
void register_cel_class(lua_State* L);
void register_cels_class(lua_State* L);
void register_color_class(lua_State* L);
void register_color_space_class(lua_State* L);
#ifdef ENABLE_UI
void register_dialog_class(lua_State* L);
#endif
void register_frame_class(lua_State* L);
void register_frames_class(lua_State* L);
void register_image_class(lua_State* L);
void register_image_iterator_class(lua_State* L);
void register_image_spec_class(lua_State* L);
void register_images_class(lua_State* L);
void register_layer_class(lua_State* L);
void register_layers_class(lua_State* L);
void register_palette_class(lua_State* L);
void register_palettes_class(lua_State* L);
void register_plugin_class(lua_State* L);
void register_point_class(lua_State* L);
void register_range_class(lua_State* L);
void register_rect_class(lua_State* L);
void register_selection_class(lua_State* L);
void register_site_class(lua_State* L);
void register_size_class(lua_State* L);
void register_slice_class(lua_State* L);
void register_slices_class(lua_State* L);
void register_sprite_class(lua_State* L);
void register_sprites_class(lua_State* L);
void register_tag_class(lua_State* L);
void register_tags_class(lua_State* L);
void register_tool_class(lua_State* L);
void register_version_class(lua_State* L);

void set_app_params(lua_State* L, const Params& params);

// We use our own fopen() that supports Unicode filename on Windows
extern "C" FILE* lua_user_fopen(const char* fname,
                                const char* mode)
{
  return base::open_file_raw(fname, mode);
}

Engine::Engine()
  : L(luaL_newstate())
  , m_delegate(nullptr)
  , m_printLastResult(false)
{
#if _DEBUG
  int top = lua_gettop(L);
#endif

  // Standard Lua libraries
  luaL_requiref(L, LUA_GNAME, luaopen_base, 1);
  luaL_requiref(L, LUA_COLIBNAME, luaopen_coroutine, 1);
  luaL_requiref(L, LUA_TABLIBNAME, luaopen_table, 1);
  luaL_requiref(L, LUA_IOLIBNAME, luaopen_io, 1);
  luaL_requiref(L, LUA_OSLIBNAME, luaopen_os, 1);
  luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1);
  luaL_requiref(L, LUA_MATHLIBNAME, luaopen_math, 1);
  luaL_requiref(L, LUA_UTF8LIBNAME, luaopen_utf8, 1);
  luaL_requiref(L, LUA_DBLIBNAME, luaopen_debug, 1);
  lua_pop(L, 9);

  // Overwrite Lua functions
  lua_register(L, "print", print);
  lua_register(L, "dofile", dofile);

  lua_getglobal(L, "os");
  for (const char* name : { "remove", "rename", "exit", "tmpname" }) {
    lua_pushcfunction(L, unsupported);
    lua_setfield(L, -2, name);
  }
  lua_pushcfunction(L, os_clock);
  lua_setfield(L, -2, "clock");
  lua_pop(L, 1);

  // Wrap io.open()
  lua_getglobal(L, "io");
  lua_getfield(L, -1, "open");
  lua_pushcclosure(L, secure_io_open, 1);
  lua_setfield(L, -2, "open");
  lua_pop(L, 1);

  // Wrap os.execute()
  lua_getglobal(L, "os");
  lua_getfield(L, -1, "execute");
  lua_pushcclosure(L, secure_os_execute, 1);
  lua_setfield(L, -2, "execute");
  lua_pop(L, 1);

  // Generic code used by metatables
  run_mt_index_code(L);

  // Register global app object
  register_app_object(L);
  register_app_pixel_color_object(L);
  register_app_fs_object(L);
  register_app_command_object(L);
  register_app_preferences_object(L);

  // Register constants
  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "ColorMode");
  setfield_integer(L, "RGB", doc::ColorMode::RGB);
  setfield_integer(L, "GRAY", doc::ColorMode::GRAYSCALE);
  setfield_integer(L, "GRAYSCALE", doc::ColorMode::GRAYSCALE);
  setfield_integer(L, "INDEXED", doc::ColorMode::INDEXED);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "AniDir");
  setfield_integer(L, "FORWARD", doc::AniDir::FORWARD);
  setfield_integer(L, "REVERSE", doc::AniDir::REVERSE);
  setfield_integer(L, "PING_PONG", doc::AniDir::PING_PONG);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "BlendMode");
  setfield_integer(L, "NORMAL", doc::BlendMode::NORMAL);
  setfield_integer(L, "MULTIPLY", doc::BlendMode::MULTIPLY);
  setfield_integer(L, "SCREEN", doc::BlendMode::SCREEN);
  setfield_integer(L, "OVERLAY", doc::BlendMode::OVERLAY);
  setfield_integer(L, "DARKEN", doc::BlendMode::DARKEN);
  setfield_integer(L, "LIGHTEN", doc::BlendMode::LIGHTEN);
  setfield_integer(L, "COLOR_DODGE", doc::BlendMode::COLOR_DODGE);
  setfield_integer(L, "COLOR_BURN", doc::BlendMode::COLOR_BURN);
  setfield_integer(L, "HARD_LIGHT", doc::BlendMode::HARD_LIGHT);
  setfield_integer(L, "SOFT_LIGHT", doc::BlendMode::SOFT_LIGHT);
  setfield_integer(L, "DIFFERENCE", doc::BlendMode::DIFFERENCE);
  setfield_integer(L, "EXCLUSION", doc::BlendMode::EXCLUSION);
  setfield_integer(L, "HSL_HUE", doc::BlendMode::HSL_HUE);
  setfield_integer(L, "HSL_SATURATION", doc::BlendMode::HSL_SATURATION);
  setfield_integer(L, "HSL_COLOR", doc::BlendMode::HSL_COLOR);
  setfield_integer(L, "HSL_LUMINOSITY", doc::BlendMode::HSL_LUMINOSITY);
  setfield_integer(L, "ADDITION", doc::BlendMode::ADDITION);
  setfield_integer(L, "SUBTRACT", doc::BlendMode::SUBTRACT);
  setfield_integer(L, "DIVIDE", doc::BlendMode::DIVIDE);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "RangeType");
  setfield_integer(L, "EMPTY", DocRange::kNone);
  setfield_integer(L, "LAYERS", DocRange::kLayers);
  setfield_integer(L, "FRAMES", DocRange::kFrames);
  setfield_integer(L, "CELS", DocRange::kCels);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "SpriteSheetType");
  setfield_integer(L, "HORIZONTAL", SpriteSheetType::Horizontal);
  setfield_integer(L, "VERTICAL", SpriteSheetType::Vertical);
  setfield_integer(L, "ROWS", SpriteSheetType::Rows);
  setfield_integer(L, "COLUMNS", SpriteSheetType::Columns);
  setfield_integer(L, "PACKED", SpriteSheetType::Packed);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "SpriteSheetDataFormat");
  setfield_integer(L, "JSON_HASH", SpriteSheetDataFormat::JsonHash);
  setfield_integer(L, "JSON_ARRAY", SpriteSheetDataFormat::JsonArray);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "BrushType");
  setfield_integer(L, "CIRCLE", doc::kCircleBrushType);
  setfield_integer(L, "SQUARE", doc::kSquareBrushType);
  setfield_integer(L, "LINE", doc::kLineBrushType);
  setfield_integer(L, "IMAGE", doc::kImageBrushType);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "BrushPattern");
  setfield_integer(L, "ORIGIN", doc::BrushPattern::ALIGNED_TO_SRC);
  setfield_integer(L, "TARGET", doc::BrushPattern::ALIGNED_TO_DST);
  setfield_integer(L, "NONE", doc::BrushPattern::PAINT_BRUSH);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "Ink");
  setfield_integer(L, "SIMPLE", app::tools::InkType::SIMPLE);
  setfield_integer(L, "ALPHA_COMPOSITING", app::tools::InkType::ALPHA_COMPOSITING);
  setfield_integer(L, "COPY_COLOR", app::tools::InkType::COPY_COLOR);
  setfield_integer(L, "LOCK_ALPHA", app::tools::InkType::LOCK_ALPHA);
  setfield_integer(L, "SHADING", app::tools::InkType::SHADING);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "FilterChannels");
  setfield_integer(L, "RED",   TARGET_RED_CHANNEL);
  setfield_integer(L, "GREEN", TARGET_GREEN_CHANNEL);
  setfield_integer(L, "BLUE",  TARGET_BLUE_CHANNEL);
  setfield_integer(L, "ALPHA", TARGET_ALPHA_CHANNEL);
  setfield_integer(L, "GRAY",  TARGET_GRAY_CHANNEL);
  setfield_integer(L, "INDEX", TARGET_INDEX_CHANNEL);
  setfield_integer(L, "RGB",   TARGET_RED_CHANNEL | TARGET_GREEN_CHANNEL | TARGET_BLUE_CHANNEL);
  setfield_integer(L, "RGBA",   TARGET_RED_CHANNEL | TARGET_GREEN_CHANNEL | TARGET_BLUE_CHANNEL | TARGET_ALPHA_CHANNEL);
  setfield_integer(L, "GRAYA",   TARGET_GRAY_CHANNEL | TARGET_ALPHA_CHANNEL);
  lua_pop(L, 1);

  lua_newtable(L);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "MouseButton");
  setfield_integer(L, "NONE",   (int)ui::kButtonNone);
  setfield_integer(L, "LEFT",   (int)ui::kButtonLeft);
  setfield_integer(L, "RIGHT",  (int)ui::kButtonRight);
  setfield_integer(L, "MIDDLE", (int)ui::kButtonMiddle);
  setfield_integer(L, "X1",     (int)ui::kButtonX1);
  setfield_integer(L, "X2",     (int)ui::kButtonX2);
  lua_pop(L, 1);

  // Register classes/prototypes
  register_brush_class(L);
  register_cel_class(L);
  register_cels_class(L);
  register_color_class(L);
  register_color_space_class(L);
#ifdef ENABLE_UI
  register_dialog_class(L);
#endif
  register_frame_class(L);
  register_frames_class(L);
  register_image_class(L);
  register_image_iterator_class(L);
  register_image_spec_class(L);
  register_images_class(L);
  register_layer_class(L);
  register_layers_class(L);
  register_palette_class(L);
  register_palettes_class(L);
  register_plugin_class(L);
  register_point_class(L);
  register_range_class(L);
  register_rect_class(L);
  register_selection_class(L);
  register_site_class(L);
  register_size_class(L);
  register_slice_class(L);
  register_slices_class(L);
  register_sprite_class(L);
  register_sprites_class(L);
  register_tag_class(L);
  register_tags_class(L);
  register_tool_class(L);
  register_version_class(L);

  // Check that we have a clean start (without dirty in the stack)
  ASSERT(lua_gettop(L) == top);
}

Engine::~Engine()
{
#ifdef ENABLE_UI
  close_all_dialogs();
#endif
  lua_close(L);
}

void Engine::printLastResult()
{
  m_printLastResult = true;
}

bool Engine::evalCode(const std::string& code,
                      const std::string& filename)
{
  bool ok = true;
  try {
    if (luaL_loadbuffer(L, code.c_str(), code.size(), filename.c_str()) ||
        lua_pcall(L, 0, 1, 0)) {
      const char* s = lua_tostring(L, -1);
      if (s)
        onConsolePrint(s);
      ok = false;
      m_returnCode = -1;
    }
    else {
      // Return code
      if (lua_isinteger(L, -1))
        m_returnCode = lua_tointeger(L, -1);
      else
        m_returnCode = 0;

      // Code was executed correctly
      if (m_printLastResult) {
        if (!lua_isnone(L, -1)) {
          const char* result = lua_tostring(L, -1);
          if (result)
            onConsolePrint(result);
        }
      }
    }
    lua_pop(L, 1);
  }
  catch (const std::exception& ex) {
    onConsolePrint(ex.what());
    ok = false;
    m_returnCode = -1;
  }

  // Collect script garbage.
  lua_gc(L, LUA_GCCOLLECT);
  return ok;
}

bool Engine::evalFile(const std::string& filename,
                      const Params& params)
{
  std::stringstream buf;
  {
    std::ifstream s(FSTREAM_PATH(filename));
    // Returns false if we cannot open the file
    if (!s)
      return false;
    buf << s.rdbuf();
  }
  std::string absFilename = base::get_absolute_path(filename);

  AddScriptFilename add(absFilename);
  set_app_params(L, params);
  return evalCode(buf.str(), "@" + absFilename);
}

void Engine::onConsolePrint(const char* text)
{
  if (!text)
    return;

  if (m_delegate)
    m_delegate->onConsolePrint(text);
  else {
    std::printf("%s\n", text);
    std::fflush(stdout);
  }
}

} // namespace script
} // namespace app
