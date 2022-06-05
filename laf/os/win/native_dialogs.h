// LAF OS Library
// Copyright (C) 2020-2021  Igara Studio S.A.
// Copyright (C) 2015  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_WIN_NATIVE_DIALOGS_H_INCLUDED
#define OS_WIN_NATIVE_DIALOGS_H_INCLUDED
#pragma once

#include "os/native_dialogs.h"

namespace os {

  class NativeDialogsWin : public NativeDialogs {
  public:
    NativeDialogsWin();
    FileDialogRef makeFileDialog() override;
  };

} // namespace os

#endif
