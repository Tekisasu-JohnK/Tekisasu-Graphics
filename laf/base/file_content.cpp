// LAF Base Library
// Copyright (C) 2018-2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/file_content.h"

#include "base/file_handle.h"

#include <algorithm>
#include <cstdio>
#include <stdexcept>

namespace base {

const size_t kChunkSize = 1024*64; // 64k

buffer read_file_content(const std::string& filename)
{
  FileHandle f(open_file(filename, "rb"));

  buffer buf;
  size_t pos = 0;

  while (!std::feof(f.get())) {
    buf.resize(buf.size() + kChunkSize);
    size_t read_bytes = std::fread(&buf[pos], 1, kChunkSize, f.get());
    pos += read_bytes;
    if (read_bytes < kChunkSize) {
      buf.resize(pos);
      break;
    }
  }

  return buf;
}


void write_file_content(const std::string& filename, const buffer& buf)
{
  write_file_content(filename, &buf[0], buf.size());
}

void write_file_content(const std::string& filename, const uint8_t* buf, size_t size)
{
  FileHandle f(open_file(filename, "wb"));

  for (size_t pos=0; pos < size; ) {
    const int write_bytes = std::min(kChunkSize, size-pos);
    const size_t written_bytes = std::fwrite(buf, 1, write_bytes, f.get());
    if (written_bytes < write_bytes)
      throw std::runtime_error("Cannot write all bytes");
    pos += written_bytes;
    buf += written_bytes;
  }
}

} // namespace base
