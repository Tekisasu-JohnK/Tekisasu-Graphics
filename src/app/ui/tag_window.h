// Aseprite
// Copyright (C) 2019-2020  Igara Studio S.A.
// Copyright (C) 2001-2017  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_UI_TAG_WINDOW_H_INCLUDED
#define APP_UI_TAG_WINDOW_H_INCLUDED
#pragma once

#include "app/ui/color_button.h"
#include "app/ui/user_data_view.h"
#include "doc/anidir.h"
#include "doc/frame.h"
#include "doc/user_data.h"

#include "tag_properties.xml.h"

namespace doc {
  class Sprite;
  class Tag;
}

namespace app {

  class TagWindow : protected app::gen::TagProperties {
  public:
    TagWindow(const doc::Sprite* sprite, const doc::Tag* tag);

    bool show();

    std::string nameValue();
    void rangeValue(doc::frame_t& from, doc::frame_t& to);
    doc::AniDir aniDirValue();
    const doc::UserData& userDataValue() const { return m_userDataView.userData(); }

  private:
    void onToggleUserData();

    const doc::Sprite* m_sprite;
    int m_base;
    doc::UserData m_userData;
    UserDataView m_userDataView;
  };

}

#endif
