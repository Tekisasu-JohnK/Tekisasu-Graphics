// LAF Library
// Copyright (c) 2019-2020  Igara Studio S.A.
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

os::Font* font = nullptr;

void draw_display(os::Display* display,
                  const gfx::Point& mousePos)
{
  os::Surface* surface = display->getSurface();
  os::SurfaceLock lock(surface);
  const gfx::Rect rc = surface->bounds();

  os::Surface* backSurface = os::instance()->createSurface(rc.w, rc.h);
  os::SurfaceLock lock2(backSurface);

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
    base::utf8_const utf8(s);
    os::draw_text(
      backSurface, font,
      utf8.begin(), utf8.end(),
      gfx::rgba(255, 255, 255), gfx::ColorNone,
      pos.x, pos.y,
      &delegate);

    pos.y += font->height() + 4;
  }

  // Flip the back surface to the display surface
  surface->drawSurface(backSurface, 0, 0);

  // Invalidates the whole display to show it on the screen.
  display->invalidateRegion(gfx::Region(rc));
}

int app_main(int argc, char* argv[])
{
  os::SystemHandle system(os::create_system());
  system->setAppMode(os::AppMode::GUI);

  os::DisplayHandle display(system->createDisplay(400, 300, 1));

  // TODO use new fonts (SkFont wrappers with system->fontManager())
  font = os::instance()->loadTrueTypeFont("/Library/Fonts/Arial Unicode.ttf", 32);
  if (!font) {
    std::printf("Font not found\n");
    return 1;
  }

  display->setNativeMouseCursor(os::kArrowCursor);
  display->setTitle("CTL");

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
      draw_display(display, mousePos);
    }
    // Wait for an event in the queue, the "true" parameter indicates
    // that we'll wait for a new event, and the next line will not be
    // processed until we receive a new event. If we use "false" and
    // there is no events in the queue, we receive an "ev.type() == Event::None
    os::Event ev;
    queue->getEvent(ev, true);

    switch (ev.type()) {

      case os::Event::CloseDisplay:
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
            display->setScale(1 + (int)(ev.scancode() - os::kKey1));
            redraw = true;
            break;
          default:
            // Do nothing for other cases
            break;
        }
        break;

      case os::Event::ResizeDisplay:
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
