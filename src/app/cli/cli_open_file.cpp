// Aseprite
// Copyright (C) 2019-2022  Igara Studio S.A.
// Copyright (C) 2016-2017  David Capello
//
// This program is distributed under the terms of
// the End-User License Agreement for Aseprite.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "app/cli/cli_open_file.h"

#include "app/doc.h"
#include "app/file/file.h"
#include "doc/sprite.h"
#include "doc/tag.h"
#include "doc/tags.h"

namespace app {

FileOpROI CliOpenFile::roi() const
{
  ASSERT(document);

  SelectedFrames selFrames;
  if (hasFrameRange())
    selFrames.insert(fromFrame, toFrame);

  return FileOpROI(document,
                   gfx::Rect(),
                   slice,
                   tag,
                   selFrames,
                   true);
}

} // namespace app
