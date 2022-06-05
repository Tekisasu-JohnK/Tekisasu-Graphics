// LAF Base Library
// Copyright (c) 2020-2021 Igara Studio S.A.
// Copyright (c) 2001-2018 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/file_handle.h"

#include "base/string.h"

#include <stdexcept>

#if LAF_WINDOWS
  #include <windows.h>
  #include <io.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#ifndef O_BINARY
#define O_BINARY  0
#define O_TEXT    0
#endif

using namespace std;

namespace base {

static void fclose_if_valid(FILE* f)
{
  if (f)
    fclose(f);
}

static void throw_cannot_open_exception(const string& filename, const string& mode)
{
  if (mode.find('w') != string::npos)
    throw std::runtime_error("Cannot save file " + filename + " in the given location");
  else
    throw std::runtime_error("Cannot open file " + filename);
}

FILE* open_file_raw(const string& filename, const string& mode)
{
#if LAF_WINDOWS
  return _wfopen(from_utf8(filename).c_str(),
                 from_utf8(mode).c_str());
#else
  return fopen(filename.c_str(), mode.c_str());
#endif
}

FileHandle open_file(const string& filename, const string& mode)
{
  return FileHandle(open_file_raw(filename, mode), fclose_if_valid);
}

FileHandle open_file_with_exception(const string& filename, const string& mode)
{
  FileHandle f(open_file_raw(filename, mode), fclose_if_valid);
  if (!f)
    throw_cannot_open_exception(filename, mode);
  return f;
}

FileHandle open_file_with_exception_sync_on_close(const std::string& filename, const std::string& mode)
{
  FileHandle f(open_file_raw(filename, mode), close_file_and_sync);
  if (!f)
    throw_cannot_open_exception(filename, mode);
  return f;
}

int open_file_descriptor_with_exception(const string& filename, const string& mode)
{
  int flags = 0;
  if (mode.find('r') != string::npos) flags |= O_RDONLY;
  if (mode.find('w') != string::npos) flags |= O_RDWR | O_CREAT | O_TRUNC;
  if (mode.find('b') != string::npos) flags |= O_BINARY;

  int fd;
#if LAF_WINDOWS
  fd = _wopen(from_utf8(filename).c_str(), flags, _S_IREAD | _S_IWRITE);
#else
  fd = open(filename.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
#endif

  if (fd == -1)
    throw_cannot_open_exception(filename, mode);

  return fd;
}

void sync_file_descriptor(int fd)
{
#if LAF_WINDOWS
  HANDLE handle = (HANDLE)_get_osfhandle(fd);
  if (handle)
    FlushFileBuffers(handle);
#endif
}

void close_file_and_sync(FILE* file)
{
  if (!file)
    return;

  fflush(file);
#if LAF_WINDOWS
  int fd = _fileno(file);
  if (fd)
    sync_file_descriptor(fd);
#endif
  fclose(file);
}

}
