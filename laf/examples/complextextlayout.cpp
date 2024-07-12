// LAF Library
// Copyright (c) 2019-2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "os/os.h"

#include <cstdio>

class MyDrawTextDelegate : public os::DrawTextDelegate {
  gfx::Point m_mousePos;
public:
  MyDrawTextDelegate(const gfx::Point& mousePos) : m_mousePos(mousePos) { }

  void preProcessChar(const int index,
                      const int codepoint,
                      gfx::Color& fg,
                      gfx::Color& bg,
                      const gfx::Rect& charBounds) override {
    if (charBounds.contains(m_mousePos)) {
      fg = gfx::rgba(0, 0, 0);
      bg = gfx::rgba(255, 255, 255);
    }
    else {
      fg = gfx::rgba(255, 255, 255);
      bg = gfx::rgba(0, 0, 0, 0);
    }
  }
};

os::FontRef font = nullptr;

void draw_window(os::Window* window,
                  const gfx::Point& mousePos)
{
  os::Surface* surface = window->surface();
  os::SurfaceLock lock(surface);
  const gfx::Rect rc = surface->bounds();

  os::SurfaceRef backSurface = os::instance()->makeSurface(rc.w, rc.h);
  os::SurfaceLock lock2(backSurface.get());

  os::Paint p;
  p.color(gfx::rgba(0, 0, 0));
  p.style(os::Paint::Fill);
  backSurface->drawRect(rc, p);

  p.color(gfx::rgba(255, 255, 255));

  const wchar_t* lines[] = { L"English",
                             L"Русский язык", // Russian
                             L"汉语",         // Simplified Chinese
                             L"日本語",       // Japanese
                             L"한국어",       // Korean
                             L"العَرَبِيَّة‎" };     // Arabic

  MyDrawTextDelegate delegate(mousePos);
  gfx::Point pos(0, 0);
  for (auto line : lines) {
    std::string s = base::to_utf8(line);
    os::draw_text(
      backSurface.get(), font.get(), s,
      gfx::rgba(255, 255, 255), gfx::ColorNone,
      pos.x, pos.y,
      &delegate);

    pos.y += font->height() + 4;
  }

  // Flip the back surface to the window surface
  surface->drawSurface(backSurface.get(), 0, 0);

  // Invalidates the whole window to show it on the screen.
  if (window->isVisible())
    window->invalidateRegion(gfx::Region(rc));
  else
    window->setVisible(true);
}

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::GUI);

  os::WindowRef window = system->makeWindow(400, 300);

  // TODO use new fonts (SkFont wrappers with system->fontManager())
  font = os::instance()->loadTrueTypeFont("/Library/Fonts/Arial Unicode.ttf", 32);
  if (!font) {
    std::printf("Font not found\n");
    return 1;
  }

  window->setTitle("CTL");

  system->finishLaunching();
  system->activateApp();

  // Wait until a key is pressed or the window is closed
  os::EventQueue* queue = system->eventQueue();
  gfx::Point mousePos;
  bool running = true;
  bool redraw = true;
  while (running) {
    if (redraw) {
      redraw = false;
      draw_window(window.get(), mousePos);
    }
    // Wait for an event in the queue, the "true" parameter indicates
    // that we'll wait for a new event, and the next line will not be
    // processed until we receive a new event. If we use "false" and
    // there is no events in the queue, we receive an "ev.type() == Event::None
    os::Event ev;
    queue->getEvent(ev);

    switch (ev.type()) {

      case os::Event::CloseWindow:
        running = false;
        break;

      case os::Event::KeyDown:
        switch (ev.scancode()) {
          case os::kKeyEsc:
            running = false;
            break;
          case os::kKey1:
          case os::kKey2:
          case os::kKey3:
          case os::kKey4:
          case os::kKey5:
          case os::kKey6:
          case os::kKey7:
          case os::kKey8:
          case os::kKey9:
            // Set scale
            window->setScale(1 + (int)(ev.scancode() - os::kKey1));
            redraw = true;
            break;
          default:
            // Do nothing for other cases
            break;
        }
        break;

      case os::Event::ResizeWindow:
        redraw = true;
        break;

      case os::Event::MouseEnter:
      case os::Event::MouseMove:
        mousePos = ev.position();
        redraw = true;
        break;

      case os::Event::MouseLeave:
        mousePos = gfx::Point(-1, -1);
        redraw = true;
        break;

      default:
        // Do nothing
        break;
    }
  }

  return 0;
}
