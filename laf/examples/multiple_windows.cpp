// LAF Library
// Copyright (c) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "gfx/hsv.h"
#include "gfx/rgb.h"
#include "os/os.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <vector>

static std::vector<os::WindowRef> windows;

const char* lines[] = { "A: Switch mouse cursor to Arrow <-> Move",
                        "H: Hide window (or show all windows again)",
                        "",
                        "C: Change window frame to content",
                        "F: Change window content to frame",
                        "W: Change window content to workarea",
                        "",
                        "D: Duplicate window",
                        "",
                        "Q: Close all windows",
                        "ESC: Close this window" };

static void redraw_window(os::Window* window)
{
  os::Surface* s = window->surface();
  os::Paint paint;
  paint.color(gfx::rgba(0, 0, 0));
  s->drawRect(window->bounds(), paint);

  paint.color(gfx::rgba(255, 255, 255));

  char buf[256];
  int y = 12;

  gfx::Rect rc = window->frame();
  std::sprintf(buf, "Frame = (%d %d %d %d)", rc.x, rc.y, rc.w, rc.h);
  os::draw_text(s, nullptr, buf, gfx::Point(0, y), &paint);
  y += 12;

  rc = window->contentRect();
  std::sprintf(buf, "Content Rect = (%d %d %d %d)", rc.x, rc.y, rc.w, rc.h);
  os::draw_text(s, nullptr, buf, gfx::Point(0, y), &paint);
  y += 12;

  for (auto line : lines) {
    y += 12;
    os::draw_text(s, nullptr, line, gfx::Point(0, y), &paint);
  }

  paint.style(os::Paint::Style::Stroke);
  s->drawRect(window->bounds(), paint);
}

static os::WindowRef add_window(const std::string& title,
                                  const os::WindowSpec& spec)
{
  os::WindowRef newWindow = os::instance()->makeWindow(spec);
  newWindow->setCursor(os::NativeCursor::Arrow);
  newWindow->setTitle(title);
  windows.emplace_back(newWindow);

  redraw_window(newWindow.get());
  newWindow->setVisible(true);
  return newWindow;
}

static void check_show_all_windows()
{
  // If all windows are hidden, show then again
  auto hidden = std::count_if(windows.begin(), windows.end(),
                              [](os::WindowRef window){
                                return !window->isVisible();
                              });
  if (hidden == windows.size()) {
    std::for_each(windows.begin(), windows.end(),
                  [](os::WindowRef window){
                    window->setVisible(true);
                  });
  }
}

static void destroy_window(const os::WindowRef& window)
{
  auto it = std::find(windows.begin(), windows.end(), window);
  if (it != windows.end())
    windows.erase(it);

  check_show_all_windows();
}

int app_main(int argc, char* argv[])
{
  auto system = os::make_system();
  system->setAppMode(os::AppMode::GUI);
  system->handleWindowResize = redraw_window;

  // Create four windows for each screen with the bounds of the
  // workarea.
  os::ScreenList screens;
  system->listScreens(screens);
  char chr = 'A';
  for (os::ScreenRef& screen : screens) {
    os::WindowSpec spec;
    spec.titled(true);
    spec.position(os::WindowSpec::Position::Frame);
    spec.frame(screen->workarea());
    spec.screen(screen);

    gfx::PointF pos[4] = { gfx::PointF(0.0, 0.0),
                           gfx::PointF(0.5, 0.0),
                           gfx::PointF(0.0, 0.5),
                           gfx::PointF(0.5, 0.5) };
    for (auto& p : pos) {
      os::WindowSpec s = spec;
      gfx::Rect frame = s.frame();
      frame.x += frame.w*p.x;
      frame.y += frame.h*p.y;
      frame.w /= 2;
      frame.h /= 2;
      s.frame(frame);
      add_window(std::string(1, chr++), s);
    }
  }

  system->finishLaunching();
  system->activateApp();

  os::EventQueue* queue = system->eventQueue();
  os::Event ev;
  while (!windows.empty()) {
    queue->getEvent(ev);

    switch (ev.type()) {

      case os::Event::CloseApp:
        windows.clear(); // Close all windows
        break;

      case os::Event::CloseWindow:
        destroy_window(ev.window());
        break;

      case os::Event::ResizeWindow:
        redraw_window(ev.window().get());
        ev.window()->invalidate();
        break;

      case os::Event::KeyDown:
        switch (ev.scancode()) {

          case os::kKeyQ:
            windows.clear();
            break;

          case os::kKeyEsc:
            destroy_window(ev.window());
            break;

          // Switch between Arrow/Move cursor in this specific window
          case os::kKeyA:
            ev.window()->setCursor(
              ev.window()->nativeCursor() == os::NativeCursor::Arrow ?
                os::NativeCursor::Move:
                os::NativeCursor::Arrow);
            break;

          case os::kKeyH:
            ev.window()->setVisible(!ev.window()->isVisible());
            check_show_all_windows();
            break;

          // Duplicate window
          case os::kKeyD: {
            std::string title = ev.window()->title();
            os::WindowSpec spec;
            spec.position(os::WindowSpec::Position::Frame);
            spec.frame(ev.window()->frame());
            add_window(title, spec);
            break;
          }

          case os::kKeyF:
          case os::kKeyC:
          case os::kKeyW: {
            std::string title = ev.window()->title();
            os::WindowSpec spec;
            if (ev.scancode() == os::kKeyF) {
              spec.position(os::WindowSpec::Position::ContentRect);
              spec.contentRect(ev.window()->frame());
            }
            else if (ev.scancode() == os::kKeyC) {
              spec.position(os::WindowSpec::Position::Frame);
              spec.frame(ev.window()->contentRect());
            }
            else if (ev.scancode() == os::kKeyW) {
              spec.position(os::WindowSpec::Position::Frame);
              spec.frame(ev.window()->screen()->workarea());
            }

            // TODO add a new os::Window::setSpec() method instead of re-creating window
            destroy_window(ev.window());
            add_window(title, spec);
            break;
          }

          // With arrow keys we can thest the os::Window::setFrame() function
          case os::kKeyLeft:
          case os::kKeyUp:
          case os::kKeyRight:
          case os::kKeyDown: {
            gfx::Rect rc = ev.window()->frame();
            switch (ev.scancode()) {
              case os::kKeyLeft:  rc.x -= rc.w; break;
              case os::kKeyUp:    rc.y -= rc.h; break;
              case os::kKeyRight: rc.x += rc.w; break;
              case os::kKeyDown:  rc.y += rc.h; break;
            }
            ev.window()->setFrame(rc);

            // Redraw window because so we can show the new position
            // on it
            redraw_window(ev.window().get());
            ev.window()->invalidate();
            break;
          }

        }
        break;

      default:
        // Do nothing
        break;
    }
  }

  return 0;
}
