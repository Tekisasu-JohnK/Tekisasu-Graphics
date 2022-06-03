// Aseprite
// Copyright (C) 2018-2020  Igara Studio S.A.
// Copyright (C) 2001-2018  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_SCRIPTING_H_INCLUDED
#define APP_SCRIPTING_H_INCLUDED
#pragma once

#ifndef ENABLE_SCRIPTING
  #error ENABLE_SCRIPTING must be defined
#endif

#include "app/color.h"
#include "app/commands/params.h"
#include "app/extensions.h"
#include "doc/brush.h"
#include "doc/frame.h"
#include "doc/object_ids.h"
#include "doc/pixel_format.h"
#include "gfx/fwd.h"

#include <cstdio>
#include <string>
#include <map>

struct lua_State;

namespace base {
  class Version;
}

namespace gfx {
  class ColorSpace;
}

namespace doc {
  class Cel;
  class Image;
  class Layer;
  class LayerGroup;
  class Mask;
  class Palette;
  class Sprite;
  class Tag;
  class WithUserData;
}

namespace app {

  class Site;

  namespace tools {
    class Tool;
  }

  namespace script {

  enum class FileAccessMode {
    Execute = 1,
    Write = 2,
    Read = 4,
    Full = 7
  };

  class EngineDelegate {
  public:
    virtual ~EngineDelegate() { }
    virtual void onConsolePrint(const char* text) = 0;
  };

  class Engine {
  public:
    Engine();
    ~Engine();

    EngineDelegate* delegate() { return m_delegate; }
    void setDelegate(EngineDelegate* delegate) {
      m_delegate = delegate;
    }

    void printLastResult();
    bool evalCode(const std::string& code,
                  const std::string& filename = std::string());
    bool evalFile(const std::string& filename,
                  const Params& params = Params());

    void consolePrint(const char* text) {
      onConsolePrint(text);
    }

    int returnCode() const {
      return m_returnCode;
    }

    lua_State* luaState() { return L; }

  private:
    void onConsolePrint(const char* text);

    lua_State* L;
    EngineDelegate* m_delegate;
    bool m_printLastResult;
    int m_returnCode;
  };

  class ScopedEngineDelegate {
  public:
    ScopedEngineDelegate(Engine* engine, EngineDelegate* delegate)
      : m_engine(engine),
        m_oldDelegate(engine->delegate()) {
      m_engine->setDelegate(delegate);
    }
    ~ScopedEngineDelegate() {
      m_engine->setDelegate(m_oldDelegate);
    }
  private:
    Engine* m_engine;
    EngineDelegate* m_oldDelegate;
  };

  int push_image_iterator_function(lua_State* L, const doc::Image* image, int extraArgIndex);
  void push_brush(lua_State* L, const doc::BrushRef& brush);
  void push_cel_image(lua_State* L, doc::Cel* cel);
  void push_cels(lua_State* L, const doc::ObjectIds& cels);
  void push_cels(lua_State* L, doc::Layer* layer);
  void push_cels(lua_State* L, doc::Sprite* sprite);
  void push_color_space(lua_State* L, const gfx::ColorSpace& cs);
  void push_doc_range(lua_State* L, Site& site);
  void push_group_layers(lua_State* L, doc::LayerGroup* group);
  void push_image(lua_State* L, doc::Image* image);
  void push_images(lua_State* L, const doc::ObjectIds& images);
  void push_layers(lua_State* L, const doc::ObjectIds& layers);
  void push_palette(lua_State* L, doc::Palette* palette);
  void push_plugin(lua_State* L, Extension* ext);
  void push_sprite_cel(lua_State* L, doc::Cel* cel);
  void push_sprite_frame(lua_State* L, doc::Sprite* sprite, doc::frame_t frame);
  void push_sprite_frames(lua_State* L, doc::Sprite* sprite);
  void push_sprite_frames(lua_State* L, doc::Sprite* sprite, const std::vector<doc::frame_t>& frames);
  void push_sprite_layers(lua_State* L, doc::Sprite* sprite);
  void push_sprite_palette(lua_State* L, doc::Sprite* sprite, doc::Palette* palette);
  void push_sprite_palettes(lua_State* L, doc::Sprite* sprite);
  void push_sprite_selection(lua_State* L, doc::Sprite* sprite);
  void push_sprite_slices(lua_State* L, doc::Sprite* sprite);
  void push_sprite_tags(lua_State* L, doc::Sprite* sprite);
  void push_sprites(lua_State* L);
  void push_tool(lua_State* L, app::tools::Tool* tool);
  void push_userdata(lua_State* L, doc::WithUserData* userData);
  void push_version(lua_State* L, const base::Version& ver);

  gfx::Point convert_args_into_point(lua_State* L, int index);
  gfx::Rect convert_args_into_rect(lua_State* L, int index);
  gfx::Size convert_args_into_size(lua_State* L, int index);
  app::Color convert_args_into_color(lua_State* L, int index);
  doc::color_t convert_args_into_pixel_color(lua_State* L, int index,
                                             const doc::PixelFormat pixelFormat);
  doc::Palette* get_palette_from_arg(lua_State* L, int index);
  doc::Image* may_get_image_from_arg(lua_State* L, int index);
  doc::Image* get_image_from_arg(lua_State* L, int index);
  doc::Cel* get_image_cel_from_arg(lua_State* L, int index);
  doc::frame_t get_frame_number_from_arg(lua_State* L, int index);
  const doc::Mask* get_mask_from_arg(lua_State* L, int index);
  app::tools::Tool* get_tool_from_arg(lua_State* L, int index);
  doc::BrushRef get_brush_from_arg(lua_State* L, int index);

  // Used by App.open(), Sprite{ fromFile }, and Image{ fromFile }
  enum class LoadSpriteFromFileParam { FullAniAsSprite,
                                       OneFrameAsSprite,
                                       OneFrameAsImage };
  int load_sprite_from_file(lua_State* L, const char* filename,
                            const LoadSpriteFromFileParam param);

#ifdef ENABLE_UI
  // close all opened Dialogs before closing the UI
  void close_all_dialogs();
#endif

} // namespace script
} // namespace app

#endif
