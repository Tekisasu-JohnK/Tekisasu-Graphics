// LAF Library
// Copyright (C) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "base/clamp.h"
#include "gfx/hsv.h"
#include "gfx/rgb.h"
#include "os/os.h"

#include <cstdarg>
#include <cstdlib>
#include <string>
#include <vector>

class PanWindow {
public:
  PanWindow(os::System* system)
    : m_display(system->createDisplay(800, 600, 1))
    , m_scroll(0.0, 0.0)
    , m_zoom(1.0)
    , m_hasCapture(false) {
    m_display->setNativeMouseCursor(os::kArrowCursor);
    m_display->setTitle("Pan Viewport");

    repaint();
  }

  bool processEvent(const os::Event& ev) {
    switch (ev.type()) {

      case os::Event::CloseDisplay:
        return false;

      case os::Event::ResizeDisplay:
        repaint();
        break;

      case os::Event::MouseEnter: break;
      case os::Event::MouseLeave: break;
      case os::Event::MouseMove:
        if (m_hasCapture) {
          m_scroll = m_captureScroll
            + gfx::PointF(ev.position() - m_capturePos);
          repaint();
        }
        break;

      case os::Event::MouseDown:
        if (!m_hasCapture) {
          m_display->setNativeMouseCursor(os::kMoveCursor);
          m_display->captureMouse();
          m_hasCapture = true;
          m_capturePos = ev.position();
          m_captureScroll = m_scroll;
        }
        break;

      case os::Event::MouseUp:
        if (m_hasCapture) {
          m_display->setNativeMouseCursor(os::kArrowCursor);
          m_display->releaseMouse();
          m_hasCapture = false;
        }
        break;

      case os::Event::MouseDoubleClick:
        // Reset
        m_scroll = gfx::PointF(0.0, 0.0);
        m_zoom = 1.0;
        repaint();
        break;

      case os::Event::MouseWheel:
        if (ev.modifiers() & os::kKeyCtrlModifier) {
          int z = (ev.wheelDelta().x + ev.wheelDelta().y);
          setZoom(gfx::PointF(ev.position()),
                  m_zoom - z/10.0);
        }
        else if (ev.preciseWheel()) {
          // TODO we have plans to change the sign of wheelDelta() when preciseWheel() is true
          m_scroll += gfx::PointF(-ev.wheelDelta());
        }
        else {
          m_scroll += gfx::PointF(-ev.wheelDelta().x*m_display->width()/32,
                                  -ev.wheelDelta().y*m_display->height()/32);
        }
        repaint();
        break;

      case os::Event::TouchMagnify:
        setZoom(gfx::PointF(ev.position()),
                m_zoom + m_zoom * ev.magnification());
        break;

      case os::Event::KeyDown:
        if (ev.scancode() == os::kKeyEsc)
          return false;
        break;

      default:
        // Do nothing
        break;
    }
    return true;
  }

private:
  void repaint() {
    os::Surface* surface = m_display->getSurface();
    os::SurfaceLock lock(surface);
    const gfx::Rect rc(surface->bounds());

    os::Paint p;
    p.style(os::Paint::Fill);
    p.color(gfx::rgba(32, 32, 32, 255));
    surface->drawRect(rc, p);

    p.style(os::Paint::Stroke);
    p.color(gfx::rgba(255, 255, 200, 255));
    {
      gfx::RectF rc2(rc);
      rc2.shrink(24);
      rc2.offset(-center());
      rc2 *= m_zoom;
      rc2.offset(center());
      rc2.offset(m_scroll);
      surface->drawRect(rc2, p);

      for (int i=1; i<8; ++i) {
        int v = i * rc2.w / 8;
        surface->drawLine(int(rc2.x + v), int(rc2.y),
                          int(rc2.x + v), int(rc2.y + rc2.h), p);
        v = i * rc2.h / 8;
        surface->drawLine(int(rc2.x),         int(rc2.y + v),
                          int(rc2.x + rc2.w), int(rc2.y + v), p);
      }
    }

    {
      std::vector<char> buf(256);
      std::sprintf(&buf[0], "Scroll=%.2f %.2f  Zoom=%.2f", m_scroll.x, m_scroll.y, m_zoom);
      p.style(os::Paint::Fill);
      os::draw_text(surface, nullptr, &buf[0], gfx::Point(12, 12), &p);
    }

    m_display->invalidate();
  }

  void setZoom(const gfx::PointF& mousePos, double newZoom) {
    double oldZoom = m_zoom;
    m_zoom = base::clamp(newZoom, 0.01, 10.0);

    // To calculate the new scroll value (m_scroll), we know that the
    // mouse position (mousePos) will be the same with the old and the
    // new scroll/zoom values.
    //
    // So:
    //   pivot    = ((mousePos - m_scroll) - center()) / oldZoom
    //   mousePos = (pivot*m_zoom + center() + m_scroll)
    //
    // Then:
    //   mousePos = (mousePos - m_scroll - center()) * m_zoom / oldZoom + center() + m_scroll
    //   m_scroll = mousePos - (mousePos - m_scroll - center()) * m_zoom / oldZoom - center()
    m_scroll = mousePos - (mousePos - m_scroll - center()) * m_zoom / oldZoom - center();

    repaint();
  }

  gfx::Point center() const {
    return gfx::Point(m_display->width()/2,
                      m_display->height()/2);
  }

  os::DisplayHandle m_display;
  gfx::PointF m_scroll;
  double m_zoom;

  // To pan the viewport with drag & drop
  bool m_hasCapture;
  gfx::Point m_capturePos;
  gfx::PointF m_captureScroll;
};

int app_main(int argc, char* argv[])
{
  os::SystemHandle system(os::create_system());
  system->setAppMode(os::AppMode::GUI);

  PanWindow window(system);

  system->finishLaunching();
  system->activateApp();

  os::EventQueue* queue = system->eventQueue();
  while (true) {
    os::Event ev;
    queue->getEvent(ev, true);
    if (!window.processEvent(ev))
      break;
  }

  return 0;
}
