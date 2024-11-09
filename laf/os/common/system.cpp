// LAF OS Library
// Copyright (C) 2019-2024  Igara Studio S.A.
// Copyright (C) 2012-2018  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "os/common/system.h"

#if LAF_WINDOWS
  #include "os/win/native_dialogs.h"
#elif LAF_MACOS
  #include "os/osx/native_dialogs.h"
#elif LAF_LINUX
  #include "os/x11/native_dialogs.h"
#else
  #include "os/native_dialogs.h"
#endif

namespace os {

CommonSystem::CommonSystem()
{
}

CommonSystem::~CommonSystem()
{
  // destroyInstance() can be called multiple times by derived
  // classes.
  if (instance() == this)
    destroyInstance();
}

NativeDialogs* CommonSystem::nativeDialogs()
{
  if (!m_nativeDialogs) {
#if LAF_WINDOWS
    m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsWin);
#elif LAF_MACOS
    m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsOSX);
#elif LAF_LINUX
    m_nativeDialogs = Ref<NativeDialogs>(new NativeDialogsX11);
#endif
  }
  return m_nativeDialogs.get();
}

FontRef CommonSystem::loadSpriteSheetFont(const char* filename, int scale)
{
  SurfaceRef sheet = loadRgbaSurface(filename);
  FontRef font = nullptr;
  if (sheet) {
    sheet->applyScale(scale);
    sheet->setImmutable();
    font = SpriteSheetFont::fromSurface(sheet);
  }
  return font;
}

FontRef CommonSystem::loadTrueTypeFont(const char* filename, int height)
{
#ifdef LAF_FREETYPE
  if (!m_ft)
    m_ft.reset(new ft::Lib());
  return FontRef(load_free_type_font(*m_ft.get(), filename, height));
#else
  return nullptr;
#endif
}

KeyModifiers CommonSystem::keyModifiers()
{
  return
    (KeyModifiers)
    ((isKeyPressed(kKeyLShift) ||
      isKeyPressed(kKeyRShift) ? kKeyShiftModifier: 0) |
     (isKeyPressed(kKeyLControl) ||
      isKeyPressed(kKeyRControl) ? kKeyCtrlModifier: 0) |
     (isKeyPressed(kKeyAlt) ? kKeyAltModifier: 0) |
     (isKeyPressed(kKeyAltGr) ? (kKeyCtrlModifier | kKeyAltModifier): 0) |
     (isKeyPressed(kKeyCommand) ? kKeyCmdModifier: 0) |
     (isKeyPressed(kKeySpace) ? kKeySpaceModifier: 0) |
     (isKeyPressed(kKeyLWin) ||
      isKeyPressed(kKeyRWin) ? kKeyWinModifier: 0));
}

#if CLIP_ENABLE_IMAGE

void get_rgba32(const clip::image_spec& spec, const uint8_t* scanlineAddr, int* r, int* g, int* b, int* a)
{
  uint32_t c = *((uint32_t*)scanlineAddr);
  if (spec.alpha_mask)
    *a = ((c & spec.alpha_mask) >> spec.alpha_shift);
  else
    *a = 255;
  // The source image is in straight-alpha and makeRgbaSurface returns a
  // surface using premultiplied-alpha so we have to premultiply the
  // source values.
  *r = ((c & spec.red_mask)  >> spec.red_shift  )*(*a)/255;
  *g = ((c & spec.green_mask)>> spec.green_shift)*(*a)/255;
  *b = ((c & spec.blue_mask) >> spec.blue_shift )*(*a)/255;
}

void get_rgba24(const clip::image_spec& spec, const uint8_t* scanlineAddr, int* r, int* g, int* b, int* a)
{
  uint32_t c = *((uint32_t*)scanlineAddr);
  *r = ((c & spec.red_mask)  >> spec.red_shift);
  *g = ((c & spec.green_mask)>> spec.green_shift);
  *b = ((c & spec.blue_mask) >> spec.blue_shift);
  *a = 255;
}

void get_rgba16(const clip::image_spec& spec, const uint8_t* scanlineAddr, int* r, int* g, int* b, int* a)
{
  uint16_t c = *((uint16_t*)scanlineAddr);
  *r = (((c & spec.red_mask  )>>spec.red_shift  )*255) / (spec.red_mask  >>spec.red_shift);
  *g = (((c & spec.green_mask)>>spec.green_shift)*255) / (spec.green_mask>>spec.green_shift);
  *b = (((c & spec.blue_mask )>>spec.blue_shift )*255) / (spec.blue_mask >>spec.blue_shift);
  *a = 255;
}

SurfaceRef CommonSystem::makeSurface(const clip::image& image)
{
  const clip::image_spec spec = image.spec();

  if (spec.bits_per_pixel != 32 &&
      spec.bits_per_pixel != 24 &&
      spec.bits_per_pixel != 16)
    return nullptr;

  SurfaceRef surface = ((System*)this)->makeRgbaSurface(spec.width, spec.height);
  SurfaceFormatData sfd;
  surface->getFormat(&sfd);

  // Select color components retrieval function.
  void (*get_rgba)(const clip::image_spec&, const uint8_t*, int*, int*, int*, int*);
  switch (spec.bits_per_pixel) {
    case 32:
      get_rgba = get_rgba32;
      break;
    case 24:
      get_rgba = get_rgba24;
      break;
    case 16:
      get_rgba = get_rgba16;
      break;
  }

  for (int v=0; v<spec.height; ++v) {
    uint32_t* dst = (uint32_t*)surface->getData(0, v);
    const uint8_t* src = ((uint8_t*)image.data()) + v * spec.bytes_per_row;
    for (int u=0; u<spec.width; ++u, ++dst) {
      int r, g, b, a;
      get_rgba(spec, src, &r, &g, &b, &a);
      *dst = (r << sfd.redShift)   |
        (g << sfd.greenShift) |
        (b << sfd.blueShift)  |
        (a << sfd.alphaShift);

      src += (spec.bits_per_pixel/8);
    }
  }

  return surface;
}

#endif

void CommonSystem::destroyInstance()
{
  // We have to reset the list of all events to clear all possible
  // living WindowRef (so all window destructors are called at this
  // point, when the os::System instance is still alive).
  //
  // TODO Maybe the event queue should be inside the System instance
  //      (so when the system is deleted, the queue is
  //      deleted). Anyway we should still clear all the events
  //      before set_instance(nullptr), and we're not sure if this
  //      is possible on macOS, as some events are queued before the
  //      System instance is even created (see
  //      EventQueue::instance() comment on laf/os/event_queue.h).
  eventQueue()->clearEvents();

  set_instance(nullptr);
}

} // namespace os
