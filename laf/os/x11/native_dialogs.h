// LAF OS Library
// Copyright (C) 2023  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_X11_NATIVE_DIALOGS_H_INCLUDED
#define OS_X11_NATIVE_DIALOGS_H_INCLUDED
#pragma once

#include "os/native_dialogs.h"

namespace os {

  class NativeDialogsX11 : public NativeDialogs {
  public:
    NativeDialogsX11();
    FileDialogRef makeFileDialog() override;
  };

} // namespace os

#endif
