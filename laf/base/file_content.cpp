// LAF Base Library
// Copyright (C) 2018-2022  Igara Studio S.A.
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

#if LAF_WINDOWS
  #include <fcntl.h>
  #include <io.h>
#endif

namespace base {

const size_t kChunkSize = 1024*64; // 64k

buffer read_file_content(FILE* file)
{
  buffer buf;
  size_t pos = 0;

  while (!std::feof(file)) {
    buf.resize(buf.size() + kChunkSize);
    size_t read_bytes = std::fread(&buf[pos], 1, kChunkSize, file);
    pos += read_bytes;
    if (read_bytes < kChunkSize) {
      buf.resize(pos);
      break;
    }
  }

  return buf;
}

buffer read_file_content(const std::string& filename)
{
  FileHandle f(open_file(filename, "rb"));
  if (f)
    return read_file_content(f.get());
  else
    return buffer();
}

void write_file_content(FILE* file, const uint8_t* buf, size_t size)
{
  for (size_t pos=0; pos < size; ) {
    const int write_bytes = std::min(kChunkSize, size-pos);
    const size_t written_bytes = std::fwrite(buf, 1, write_bytes, file);
    if (written_bytes < write_bytes)
      throw std::runtime_error("Cannot write all bytes");
    pos += written_bytes;
    buf += written_bytes;
  }
}

void write_file_content(const std::string& filename, const uint8_t* buf, size_t size)
{
  FileHandle f(open_file(filename, "wb"));
  if (f)
    write_file_content(f.get(), buf, size);
}

void set_write_binary_file_content(FILE* file)
{
#if LAF_WINDOWS
  _setmode(_fileno(file), O_BINARY);
#endif
}

} // namespace base
