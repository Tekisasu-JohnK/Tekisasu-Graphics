// LAF OS Library
// Copyright (C) 2020  Igara Studio S.A.
// Copyright (C) 2017  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_GTK_NATIVE_DIALOGS_H_INCLUDED
#define OS_GTK_NATIVE_DIALOGS_H_INCLUDED
#pragma once

#include "os/native_dialogs.h"

extern "C" struct _GtkApplication;

namespace os {

  class NativeDialogsGTK : public NativeDialogs {
  public:
    NativeDialogsGTK();
    ~NativeDialogsGTK();
    FileDialogRef makeFileDialog() override;
  private:
    _GtkApplication* m_gtkApp;
  };

} // namespace os

#endif
