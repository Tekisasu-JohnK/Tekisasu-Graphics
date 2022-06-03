// Aseprite
// Copyright (C) 2001-2017  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifndef APP_UI_EDITOR_TRANSFORM_HANDLES_H_INCLUDED
#define APP_UI_EDITOR_TRANSFORM_HANDLES_H_INCLUDED
#pragma once

#include "app/transformation.h"
#include "app/ui/editor/handle_type.h"
#include "fixmath/fixmath.h"
#include "gfx/point.h"

#include <vector>

namespace ui {
  class Graphics;
}

namespace app {
  class Editor;

  // Helper class to do hit-detection and render transformation handles
  // and rotation pivot.
  class TransformHandles {
  public:
    // Returns the handle in the given mouse point (pt) when the user
    // has applied the given transformation to the selection.
    HandleType getHandleAtPoint(Editor* editor, const gfx::Point& pt, const Transformation& transform);

    void drawHandles(Editor* editor, const Transformation& transform);
    void invalidateHandles(Editor* editor, const Transformation& transform);

  private:
    gfx::Rect getPivotHandleBounds(Editor* editor,
      const Transformation& transform,
      const Transformation::Corners& corners);

    bool inHandle(const gfx::Point& pt, int x, int y, int gfx_w, int gfx_h, fixmath::fixed angle);
    void drawHandle(ui::Graphics* g, int x, int y, fixmath::fixed angle);
    void adjustHandle(int& x, int& y, int handle_w, int handle_h, fixmath::fixed angle);
    bool visiblePivot(fixmath::fixed angle) const;
    void getScreenPoints(
      Editor* editor,
      const Transformation::Corners& corners,
      std::vector<gfx::Point>& screenPoints) const;
  };

} // namespace app

#endif
