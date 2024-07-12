// LAF OS Library
// Copyright (C) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_GL_CONTEXT_NSGL_INCLUDED
#define OS_GL_CONTEXT_NSGL_INCLUDED
#pragma once

#include "os/gl/gl_context.h"

namespace os {

class GLContextNSGL : public GLContext {
public:
  GLContextNSGL();
  ~GLContextNSGL();

  void setView(id view);

  bool isValid() override;
  bool createGLContext() override;
  void destroyGLContext() override;
  void makeCurrent() override;
  void swapBuffers() override;

  id nsglContext() {
    return m_nsgl;
  }

private:
  id m_nsgl = nullptr; // NSOpenGLContext
  id m_view = nullptr; // NSView
};

} // namespace os

#endif
