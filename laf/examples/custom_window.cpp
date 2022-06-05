// LAF Library
// Copyright (c) 2021  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "gfx/path.h"
#include "os/os.h"

using Hit = os::Hit;

const int kTitleBarSize = 32;
const int kButtonSize = 32;
const int kResizeBorder = 8;

const gfx::Color kTitleBarBase = gfx::rgba(100, 100, 200);
const gfx::Color kTitleBarHigh = gfx::rgba(200, 200, 220);
const gfx::Color kTitleBarText = gfx::rgba(25, 25, 50);
const gfx::Color kContentBase = gfx::rgba(40, 40, 35);
const gfx::Color kContentHigh = gfx::rgba(50, 55, 45);
const gfx::Color kContentText = gfx::rgba(105, 115, 85);
const gfx::Color kContentEdge = gfx::rgba(200, 200, 100);

Hit hit_test(os::Window* window,
             const gfx::Point& pos)
{
  // For a full screen window, we are always in the content area
  if (window->isFullscreen())
    return Hit::Content;

  gfx::Rect rc = window->bounds();
  gfx::Rect rc2 = rc;
  rc2.shrink(kResizeBorder);

  // Mouse in client area
  if (!rc.contains(pos)) {
    return Hit::Content;
  }
  // Resize edges
  else if (!rc2.contains(pos) &&
           // macOS cannot start the resizing actions (just the window movement)
           os::instance()->hasCapability(os::Capabilities::CanStartWindowResize)) {
    if (pos.y < kResizeBorder) {
      if (pos.x < kResizeBorder) return Hit::TopLeft;
      else if (pos.x > rc.x2()-kResizeBorder) return Hit::TopRight;
      else return Hit::Top;
    }
    else if (pos.y > rc.y2()-kResizeBorder) {
      if (pos.x < kResizeBorder) return Hit::BottomLeft;
      else if (pos.x > rc.x2()-kResizeBorder) return Hit::BottomRight;
      else return Hit::Bottom;
    }
    else {
      if (pos.x < rc.w/2) return Hit::Left;
      return Hit::Right;
    }
  }
  else if (pos.y <= kTitleBarSize) {
    if (pos.x > rc.x2()-kButtonSize) return Hit::CloseButton;
    else if (pos.x > rc.x2()-kButtonSize*2) return Hit::MaximizeButton;
    else if (pos.x > rc.x2()-kButtonSize*3) return Hit::MinimizeButton;
    else return Hit::TitleBar;
  }
  else {
    return Hit::Content;
  }
}

void draw_button(os::Surface* surface, int x, Hit button, const Hit hit)
{
  os::Paint p;
  gfx::Rect box(x, 0, kButtonSize, kButtonSize);

  p.color(hit == button ? kTitleBarHigh: kTitleBarBase);
  p.style(os::Paint::Fill);
  surface->drawRect(box, p);

  p.color(gfx::rgba(25, 25, 50));
  p.style(os::Paint::Stroke);
  surface->drawRect(gfx::Rect(x, 0, 2, kButtonSize), p);

  // Draw icon
  box.shrink(11);
  box.inflate(1, 1);
  p.strokeWidth(1.5f);
  p.antialias(true);
  switch (button) {
    case Hit::MinimizeButton:
      surface->drawRect(gfx::Rect(box.x, box.y2()-2, box.w, 1), p);
      break;
    case Hit::MaximizeButton:
      surface->drawRect(gfx::Rect(box), p);
      break;
    case Hit::CloseButton: {
      gfx::Path path;
      path.moveTo(box.x, box.y);
      path.lineTo(box.x2(), box.y2());
      path.moveTo(box.x2(), box.y);
      path.lineTo(box.x, box.y2());
      surface->drawPath(path, p);
      break;
    }
  }
}

void draw_window(os::Window* window, const Hit hit)
{
  os::Surface* surface = window->surface();
  os::SurfaceLock lock(surface);
  gfx::Rect rc = surface->bounds();
  gfx::Rect rc2 = rc;
  os::Paint p;
  p.style(os::Paint::Fill);

  // Draw custom title bar area
  if (!window->isFullscreen()) {
    rc2.h = kTitleBarSize;

    p.color(hit == Hit::TitleBar ? kTitleBarHigh: kTitleBarBase);
    surface->drawRect(gfx::Rect(rc2).inflate(-kButtonSize*3, 0), p);

    rc2.y += kTitleBarSize/2 - 10;

    p.color(kTitleBarText);
    os::draw_text(surface, nullptr, "Custom Window",
                  rc2.center(), &p, os::TextAlign::Center);

    // Draw buttons
    draw_button(surface, rc.x2()-kButtonSize, Hit::CloseButton, hit);
    draw_button(surface, rc.x2()-kButtonSize*2, Hit::MaximizeButton, hit);
    draw_button(surface, rc.x2()-kButtonSize*3, Hit::MinimizeButton, hit);

    // Client area
    rc2 = rc;
    rc2.y += kTitleBarSize;
    rc2.h -= kTitleBarSize;
  }

  // Draw client area
  p.color(hit == Hit::Content ? kContentHigh: kContentBase);
  surface->drawRect(rc2, p);

  p.style(os::Paint::Style::Stroke);
  p.color(kContentEdge);
  surface->drawRect(rc2, p);

  p.style(os::Paint::Style::Fill);
  p.color(kContentText);
  os::draw_text(surface, nullptr, "Content Rect",
                rc2.center(), &p, os::TextAlign::Center);

  if (window->isFullscreen()) {
    auto pos = rc2.center();
    pos.y += 24;
    os::draw_text(surface, nullptr, "(F key or F11 to exit full screen)",
                  pos, &p, os::TextAlign::Center);
  }

  if (window->isVisible())
    window->invalidateRegion(gfx::Region(rc));
  else
    window->setVisible(true);
}

bool update_hit(os::Window* window,
                const os::Event& ev,
                Hit& hit)
{
  Hit newHit = hit_test(window, ev.position());
  if (newHit != hit) {
    hit = newHit;
    return true;
  }
  else
    return false;
}

os::WindowRef create_window()
{
  os::WindowSpec spec;
  spec.contentRect(gfx::Rect(32, 32, 400, 300));
  spec.titled(false);
  spec.borderless(true);

  os::WindowRef window = os::instance()->makeWindow(spec);
  window->setTitle("Custom Window");
  window->handleHitTest = hit_test;

  return window;
}

void handle_mouse_move(os::Window* window,
                       const Hit hit)
{
  os::NativeCursor cursor = os::NativeCursor::Arrow;
  switch (hit) {
    case Hit::Content:     cursor = os::NativeCursor::Arrow;  break;
    case Hit::TitleBar:    cursor = os::NativeCursor::Move;   break;
    case Hit::TopLeft:     cursor = os::NativeCursor::SizeNW; break;
    case Hit::Top:         cursor = os::NativeCursor::SizeN;  break;
    case Hit::TopRight:    cursor = os::NativeCursor::SizeNE; break;
    case Hit::Left:        cursor = os::NativeCursor::SizeW;  break;
    case Hit::Right:       cursor = os::NativeCursor::SizeE;  break;
    case Hit::BottomLeft:  cursor = os::NativeCursor::SizeSW; break;
    case Hit::Bottom:      cursor = os::NativeCursor::SizeS;  break;
    case Hit::BottomRight: cursor = os::NativeCursor::SizeSE; break;
    default: break;
  }
  window->setCursor(cursor);
}

bool handle_mouse_down(os::Window* window,
                       const os::Event& ev,
                       const Hit hit)
{
  os::NativeCursor cursor = os::NativeCursor::Arrow;
  os::WindowAction action = os::WindowAction::Move;
  switch (hit) {
    case Hit::Content:        return true;
    case Hit::TitleBar:       action = os::WindowAction::Move; break;
    case Hit::TopLeft:        action = os::WindowAction::ResizeFromTopLeft; break;
    case Hit::Top:            action = os::WindowAction::ResizeFromTop;  break;
    case Hit::TopRight:       action = os::WindowAction::ResizeFromTopRight; break;
    case Hit::Left:           action = os::WindowAction::ResizeFromLeft;  break;
    case Hit::Right:          action = os::WindowAction::ResizeFromRight;  break;
    case Hit::BottomLeft:     action = os::WindowAction::ResizeFromBottomLeft; break;
    case Hit::Bottom:         action = os::WindowAction::ResizeFromBottom;  break;
    case Hit::BottomRight:    action = os::WindowAction::ResizeFromBottomRight; break;
    case Hit::MinimizeButton: window->minimize(); return true;
    case Hit::MaximizeButton: window->maximize(); return true;
    case Hit::CloseButton:    return false;
  }
  window->performWindowAction(action, &ev);
  return true;
}

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::GUI);

  os::WindowRef window = create_window();
  Hit hit = Hit::None; // Current area which the mouse cursor hits
  window->activate();

  system->handleWindowResize = [&](os::Window* w) { draw_window(w, hit); };
  system->finishLaunching();
  system->activateApp();

  os::EventQueue* queue = system->eventQueue();
  bool running = true;
  bool redraw = true;
  while (running) {
    if (redraw) {
      redraw = false;
      draw_window(window.get(), hit);
    }

    os::Event ev;
    queue->getEvent(ev);

    switch (ev.type()) {

      case os::Event::CloseApp:
      case os::Event::CloseWindow:
        running = false;
        break;

      case os::Event::KeyDown:
        switch (ev.scancode()) {
          case os::kKeyEsc:
            running = false;
            break;
          case os::kKeyF:
          case os::kKeyF11:
            window->setFullscreen(!window->isFullscreen());
            break;
        }
        break;

      case os::Event::MouseEnter:
      case os::Event::MouseMove:
        redraw = update_hit(window.get(), ev, hit);
        handle_mouse_move(window.get(), hit);
        break;

      case os::Event::MouseDown:
        redraw = update_hit(window.get(), ev, hit);
        if (!handle_mouse_down(window.get(), ev, hit))
          running = false;
        break;

      case os::Event::MouseLeave:
        redraw = update_hit(window.get(), ev, hit);
        break;

      default:
        break;
    }
  }

  return 0;
}
