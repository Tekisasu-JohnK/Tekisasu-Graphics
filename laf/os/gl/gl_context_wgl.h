// LAF OS Library
// Copyright (C) 2022  Igara Studio S.A.
// Copyright (C) 2015-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_GL_CONTEXT_WGL_INCLUDED
#define OS_GL_CONTEXT_WGL_INCLUDED
#pragma once

#include "os/gl/gl_context.h"

#include <windows.h>

namespace os {

class GLContextWGL : public GLContext {
public:
  GLContextWGL(HWND hwnd)
    : m_hwnd(hwnd)
    , m_glrc(nullptr) {
  }

  ~GLContextWGL() {
    destroyGLContext();
  }

  bool isValid() override {
    return m_glrc != nullptr;
  }

  bool createGLContext() override {
    HDC olddc = wglGetCurrentDC();
    HGLRC oldglrc = wglGetCurrentContext();
    HDC hdc = GetDC(m_hwnd);

    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),
      1,                                // version number
      PFD_DRAW_TO_WINDOW |              // support window
      PFD_SUPPORT_OPENGL |              // support OpenGL
      PFD_DOUBLEBUFFER,                 // double buffered
      PFD_TYPE_RGBA,                    // RGBA type
      24,                               // 24-bit color depth
      0, 0, 0, 0, 0, 0,                 // color bits ignored
      8,                                // 8-bit alpha buffer
      0,                                // shift bit ignored
      0,                                // no accumulation buffer
      0, 0, 0, 0,                       // accum bits ignored
      0,                                // no z-buffer
      0,                                // no stencil buffer
      0,                                // no auxiliary buffer
      PFD_MAIN_PLANE,                   // main layer
      0,                                // reserved
      0, 0, 0                           // layer masks ignored
    };
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    m_glrc = wglCreateContext(hdc);
    if (!m_glrc) {
      ReleaseDC(m_hwnd, hdc);
      return false;
    }

    ReleaseDC(m_hwnd, hdc);
    return true;
  }

  void destroyGLContext() override {
    if (m_glrc) {
      wglMakeCurrent(nullptr, nullptr);
      wglDeleteContext(m_glrc);
      m_glrc = nullptr;
    }
  }

  void makeCurrent() override {
    HDC hdc = GetDC(m_hwnd);
    wglMakeCurrent(hdc, m_glrc);
    ReleaseDC(m_hwnd, hdc);
  }

  void swapBuffers() override {
    HDC hdc = GetDC(m_hwnd);
    SwapBuffers(hdc);
    ReleaseDC(m_hwnd, hdc);
  }

private:
  HWND m_hwnd;
  HGLRC m_glrc;
};

} // namespace os

#endif
