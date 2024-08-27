// LAF OS Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_DND_H_INCLUDED
#define OS_DND_H_INCLUDED
#pragma once

#include "base/paths.h"
#include "base/debug.h"
#include "gfx/point.h"
#include "os/surface.h"

#include <memory>

#pragma push_macro("None")
#undef None // Undefine the X11 None macro

namespace os {

  SurfaceRef default_decode_png(const uint8_t* buf, uint32_t len);
  SurfaceRef default_decode_jpg(const uint8_t* buf, uint32_t len);
  SurfaceRef default_decode_bmp(const uint8_t* buf, uint32_t len);
  SurfaceRef default_decode_gif(const uint8_t* buf, uint32_t len);

  class Window;

#if CLIP_ENABLE_IMAGE
  using DecoderFunc = SurfaceRef(*)(const uint8_t* buf, uint32_t len);

  // Methods used to configure custom decoder functions by replacing the default implementations.

  void set_decode_png(DecoderFunc func);
  void set_decode_jpg(DecoderFunc func);
  void set_decode_bmp(DecoderFunc func);
  void set_decode_gif(DecoderFunc func);

  SurfaceRef decode_png(const uint8_t* buf, uint32_t len);
  SurfaceRef decode_jpg(const uint8_t* buf, uint32_t len);
  SurfaceRef decode_bmp(const uint8_t* buf, uint32_t len);
  SurfaceRef decode_gif(const uint8_t* buf, uint32_t len);
#endif
  // Operations that can be supported by source and target windows in a drag
  // and drop operation.
  enum class DropOperation {
    None = 0,
    Copy = 1,
    Move = 2,
    Link = 4,
    All = Copy | Move | Link,
  };

  // Types of representations supported for each DragDataItem.
  enum class DragDataItemType {
    Paths,
    Image,
    Url,
  };

  // Interface to get dragged data from the platform's implementation.
  class DragDataProvider {
  public:
    virtual ~DragDataProvider() {}
    virtual base::paths getPaths() = 0;
#if CLIP_ENABLE_IMAGE
    virtual SurfaceRef getImage() = 0;
#endif
    virtual std::string getUrl() = 0;
    virtual bool contains(DragDataItemType type) { return false; }
  };

  class DragEvent {
  public:
    DragEvent(os::Window* target,
              DropOperation supportedOperations,
              const gfx::Point& dragPosition,
              std::unique_ptr<DragDataProvider>& dataProvider)
      : m_target(target)
      , m_supportedOperations(supportedOperations)
      , m_position(dragPosition)
      , m_dataProvider(std::move(dataProvider)) {}

    // Destination window of the DragEvent.
    os::Window* target() const { return m_target; }
    DropOperation dropResult() const { return m_dropResult; }
    DropOperation supportedOperations() const { return m_supportedOperations; }
    bool acceptDrop() const { return m_acceptDrop; }
    const gfx::Point& position() { return m_position; }
    DragDataProvider* dataProvider() { return m_dataProvider.get(); }

    // Sets what will be the outcome of dropping the dragged data when it is
    // accepted by the target window. Only one of the enum values should be passed,
    // do not combine values using bitwise operators.
    void dropResult(DropOperation operation) { m_dropResult = operation; }
    // Set this to true when the dropped data was accepted/processed by the
    // target window, or set to false otherwise.
    void acceptDrop(bool value) { m_acceptDrop = value; }

    bool sourceSupports(DropOperation op) {
      return (static_cast<int>(m_supportedOperations) & static_cast<int>(op))
        == static_cast<int>(op);
    }

  private:
    os::Window* m_target = nullptr;
    DropOperation m_dropResult = DropOperation::Copy;
    // Bitwise OR of the operations supported by the drag and drop source.
    DropOperation m_supportedOperations;
    bool m_acceptDrop = false;
    gfx::Point m_position;
    std::unique_ptr<DragDataProvider> m_dataProvider = nullptr;
  };

  class DragTarget {
  public:
    virtual ~DragTarget() {};

    // Called when a drag action enters a window that supports DnD. The
    // DragEvent::dropResult must be set to the operation that is expected
    // to occur by the target window once the drop is accepted.
    virtual void dragEnter(os::DragEvent& ev) {}
    // Called when the dragged data exits the window that supports DnD.
    virtual void dragLeave(os::DragEvent& ev) {}
    virtual void drag(os::DragEvent& ev) {}
    virtual void drop(os::DragEvent& ev) {}
  };


} // namespace os

#pragma pop_macro("None")

#endif