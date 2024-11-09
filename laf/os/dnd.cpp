// LAF OS Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#if CLIP_ENABLE_IMAGE

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "os/dnd.h"
#include "os/system.h"

namespace os {

static DecoderFunc g_decode_png = default_decode_png;
static DecoderFunc g_decode_jpg = default_decode_jpg;
static DecoderFunc g_decode_bmp = default_decode_bmp;
static DecoderFunc g_decode_gif = default_decode_gif;

void set_decode_png(DecoderFunc func)
{
  g_decode_png = func;
}

void set_decode_jpg(DecoderFunc func)
{
  g_decode_jpg = func;
}

void set_decode_bmp(DecoderFunc func)
{
  g_decode_bmp = func;
}

void set_decode_gif(DecoderFunc func)
{
  g_decode_gif = func;
}

SurfaceRef decode_png(const uint8_t* buf, const uint32_t len)
{
  return g_decode_png(buf, len);
}

SurfaceRef decode_jpg(const uint8_t* buf, const uint32_t len)
{
  return g_decode_jpg(buf, len);
}

SurfaceRef decode_bmp(const uint8_t* buf, const uint32_t len)
{
  return g_decode_bmp(buf, len);
}

SurfaceRef decode_gif(const uint8_t* buf, const uint32_t len)
{
  return g_decode_gif(buf, len);
}

} // namespace os

#endif