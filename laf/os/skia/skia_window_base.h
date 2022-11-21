// LAF OS Library
// Copyright (C) 2021-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_WINDOW_BASE_INCLUDED
#define OS_SKIA_SKIA_WINDOW_BASE_INCLUDED
#pragma once

#include "os/event.h"
#include "os/event_queue.h"
#include "os/gl/gl_context.h"
#include "os/skia/skia_gl.h"
#include "os/skia/skia_surface.h"
#include "os/system.h"
#include "os/window.h"

#include "include/core/SkCanvas.h"

#if SK_SUPPORT_GPU
  #if LAF_WINDOWS
    #include <windows.h>
    #include <GL/gl.h>
  #elif LAF_MACOS
    #include <OpenGL/gl.h>
  #endif
#endif

namespace os {

template<typename T>
class SkiaWindowBase : public T {
public:
  using Base = SkiaWindowBase<T>;

  template<typename... Args>
  SkiaWindowBase(Args&&... args)
    : T(std::forward<Args&&>(args)...)
    , m_initialized(false)
    , m_surface(new SkiaSurface)
    , m_colorSpace(nullptr) {
  }

  void initColorSpace() {
    // Needed on macOS because WindowOSX::colorSpace() needs the
    // m_nsWindow created, and that happens after
    // WindowOSX::createWindow() is called.
    m_colorSpace = T::colorSpace();
  }

  bool isInitialized() const {
    return m_initialized;
  }

  void resetSkiaSurface() {
    if (m_surface)
      m_surface.reset();

    resizeSkiaSurface(this->clientSize());
  }

  void resizeSkiaSurface(const gfx::Size& size) {
    if (!m_initialized)
      return;

    gfx::Size newSize(size.w / this->scale(),
                      size.h / this->scale());
    newSize.w = std::max(1, newSize.w);
    newSize.h = std::max(1, newSize.h);

    if (m_initialized &&
        m_surface &&
        m_surface->width() == newSize.w &&
        m_surface->height() == newSize.h) {
      return;
    }

    m_backend = Backend::NONE;
    m_surface.reset();

#if SK_SUPPORT_GPU
    // Re-create OpenGL context
    m_gl.detachGL();
    if (m_glCtx->isValid())
      m_glCtx->destroyGLContext();

    // GPU-accelerated surface
    if (os::instance()->gpuAcceleration()) {
      m_glCtx->createGLContext();

      if (m_glCtx->isValid()) {
        m_glCtx->makeCurrent();

        if (m_gl.attachGL() &&
            m_gl.createRenderTarget(
              size, this->scale(),
              ((SkiaColorSpace*)colorSpace().get())->skColorSpace())) {
          m_surface = make_ref<SkiaSurface>(m_gl.surface());
          m_backend = Backend::GL;
        }
      }
    }
#endif  // SK_SUPPORT_GPU

    // Raster surface
    if (!m_surface) {
      m_surface = make_ref<SkiaSurface>();

      if (T::isTransparent())
        m_surface->createRgba(newSize.w, newSize.h, m_colorSpace);
      else
        m_surface->create(newSize.w, newSize.h, m_colorSpace);
    }
  }

  // Returns the main surface to draw into this window.
  // You must not dispose this surface.
  Surface* surface() override {
    return m_surface.get();
  }

  // Overrides the colorSpace() method to return the cached/stored
  // color space in this instance (instead of asking for the color
  // space to the screen as T::colorSpace() should do).
  os::ColorSpaceRef colorSpace() const override {
    return m_colorSpace;
  }

  void setColorSpace(const os::ColorSpaceRef& colorSpace) override {
    if (colorSpace)
      m_colorSpace = colorSpace;
    else
      m_colorSpace = T::colorSpace(); // Screen color space

    if (m_surface)
      resetSkiaSurface();

    // Generate the resizing window event to redraw everything.
    // TODO we could create a new event like Event::ColorSpaceChange,
    // but the result would be the same, the window must be re-painted.
    Event ev;
    ev.setType(Event::ResizeWindow);
    ev.setWindow(AddRef(this));
    os::queue_event(ev);
  }

  void swapBuffers() override {
#if SK_SUPPORT_GPU
    if (m_backend == Backend::NONE ||
        !m_gl.backbufferSurface() ||
        !m_glCtx->isValid())
      return;

    auto surface = static_cast<SkiaSurface*>(this->surface());
    if (!surface)
      return;

    m_glCtx->makeCurrent();

    // Draw the small (unscaled) surface to the backbuffer surface
    // scaling it to the this->scale() factor.
    if (m_gl.backbufferSurface() != m_gl.surface()) {
      SkSamplingOptions sampling;
      SkPaint paint;

      SkCanvas* dstCanvas = m_gl.backbufferSurface()->getCanvas();
      dstCanvas->save();
      dstCanvas->scale(SkScalar(this->scale()),
                       SkScalar(this->scale()));
      m_gl.surface()->draw(
        dstCanvas,
        0.0, 0.0, sampling, &paint);
      dstCanvas->restore();
    }

    surface->flushAndSubmit();
    m_glCtx->swapBuffers();
#endif // SK_SUPPORT_GPU
  }

  bool isGpuAccelerated() const override {
#if SK_SUPPORT_GPU
    return (m_backend == Backend::GL);
#else
    return false;
#endif
  }

#if SK_SUPPORT_GPU
  GrDirectContext* sk_grCtx() const override {
    return m_gl.grCtx();
  }
#endif

protected:
  void initializeSurface() {
    m_initialized = true;
    resetSkiaSurface();
  }

  void onResize(const gfx::Size& sz) override {
    resizeSkiaSurface(sz);

    if (os::instance()->handleWindowResize &&
        // Check that the surface is created to avoid a call to
        // handleWindowResize() with an empty surface (or null
        // SkiaSurface::m_canvas) when the window is being created.
        isInitialized()) {
      os::instance()->handleWindowResize(this);
    }
    else {
      Event ev;
      ev.setType(Event::ResizeWindow);
      ev.setWindow(AddRef(this));
      queue_event(ev);
    }
  }

  enum class Backend {
    NONE,
#if SK_SUPPORT_GPU
    GL,
#endif
  };

  Backend backend() const { return m_backend; }

#if SK_SUPPORT_GPU
  std::unique_ptr<GLContext> m_glCtx;
  SkiaGL m_gl;
#endif

private:
  Backend m_backend = Backend::NONE;
  // Flag used to avoid accessing to an invalid m_surface in the first
  // SkiaWindow::resize() call when the window is created (as the
  // window is created, it send a first resize event.)
  bool m_initialized;
  Ref<SkiaSurface> m_surface;
  os::ColorSpaceRef m_colorSpace;
};

} // namespace os

#endif
