// LAF OS Library
// Copyright (C) 2022  Igara Studio S.A.
// Copyright (C) 2015-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_GL_CONTEXT_INCLUDED
#define OS_GL_CONTEXT_INCLUDED
#pragma once

namespace os {

class GLContext {
public:
  virtual ~GLContext() { }
  virtual bool isValid() { return false; }
  virtual bool createGLContext() { return false; }
  virtual void destroyGLContext() { }
  virtual void makeCurrent() { }
  virtual void swapBuffers() { }
};

} // namespace os

#endif
