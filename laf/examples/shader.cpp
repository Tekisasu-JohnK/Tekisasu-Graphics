// LAF Library
// Copyright (C) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/os.h"

#ifndef LAF_SKIA
  #error You need Skia library to compile this example
#endif

#if !SK_SUPPORT_GPU
  #error You need Skia with GPU support to compile this example
#endif

#include "base/time.h"
#include "os/skia/skia_surface.h"

#include "include/core/SkCanvas.h"
#include "include/effects/SkRuntimeEffect.h"

#include <cstdio>

base::tick_t startTick;

// Shader from:
//   https://shaders.skia.org/
//   https://twitter.com/notargs/status/1250468645030858753
const char* shaderCode = R"(
uniform float3 iResolution;
uniform float  iTime;

float f(vec3 p) {
  p.z -= iTime * 10.0;
  float a = p.z * .1;
  p.xy *= mat2(cos(a), sin(a), -sin(a), cos(a));
  return .1 - length(cos(p.xy) + sin(p.yz));
}

half4 main(vec2 fragcoord) {
  vec3 d = .5 - fragcoord.xy1 / iResolution.y;
  vec3 p = vec3(0);
  for (int i = 0; i < 32; i++) {
    p += f(p) * d;
  }
  return ((sin(p) + vec3(2, 5, 9)) / length(p)).xyz1;
}
)";

class ShaderWindow {
public:
  ShaderWindow(os::System* system)
    : m_builder(SkRuntimeEffect::MakeForShader(SkString(shaderCode)).effect) {
    m_window = system->makeWindow(256, 256);
    m_window->setCursor(os::NativeCursor::Arrow);
    m_window->setTitle("Shader");
    repaint();
    m_window->setVisible(true);
  }

  bool processEvent(const os::Event& ev) {
    switch (ev.type()) {

      case os::Event::CloseWindow:
        return false;

      case os::Event::ResizeWindow:
        repaint();
        break;

      case os::Event::KeyDown:
        if (ev.scancode() == os::kKeyEsc)
          return false;
        else if (ev.scancode() == os::kKeyG) {
          os::instance()->setGpuAcceleration(
            !os::instance()->gpuAcceleration());
        }
        break;

      default:
        // Do nothing
        break;
    }
    return true;
  }

  void repaint() {
    os::Surface* surface = m_window->surface();
    os::SurfaceLock lock(surface);

    SkCanvas* canvas = &static_cast<os::SkiaSurface*>(surface)->canvas();
    skiaPaint(canvas);

    if (m_window->isGpuAccelerated()) {
      os::Paint p;
      p.color(gfx::rgba(0, 0, 0));
      os::draw_text(surface, nullptr, "GPU", gfx::Point(12, 12),
                    &p, os::TextAlign::Center);
    }

    m_window->invalidate();
    m_window->swapBuffers();
  }

private:

  void skiaPaint(SkCanvas* canvas) {
    SkImageInfo ii = canvas->imageInfo();
    m_builder.uniform("iResolution") = SkV3{float(ii.width()),
                                            float(ii.height()), 0.0f};
    m_builder.uniform("iTime") =
      float((base::current_tick() - startTick) / 1000.0f);

    SkPaint p;
    p.setShader(m_builder.makeShader());
    canvas->drawPaint(p);
  }

  os::WindowRef m_window;
  SkRuntimeShaderBuilder m_builder;
};

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::GUI);
  system->setGpuAcceleration(true);

  ShaderWindow window(system.get());

  system->handleWindowResize = [&window](os::Window* win){
    window.repaint();
  };

  system->finishLaunching();
  system->activateApp();

  startTick = base::current_tick();

  os::EventQueue* queue = system->eventQueue();
  auto t = startTick;
  double paintDelay = 0.0;

  while (true) {
    os::Event ev;

    ASSERT(paintDelay >= 0.0);
    const double waitSecs =
      (base::current_tick() - t) / 1000.0 * 60.0 + paintDelay;

    queue->getEvent(ev, waitSecs);
    if (!window.processEvent(ev))
      break;

    auto now = base::current_tick();
    paintDelay -= (now - t) / 1000.0;
    if (paintDelay < 0.0) {
      auto paintStart = now;

      window.repaint();

      now = base::current_tick();
      paintDelay = (now - paintStart) / 1000.0;
    }
    t = now;
  }

  return 0;
}
