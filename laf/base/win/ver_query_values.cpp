// LAF Base Library
// Copyright (c) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/win/ver_query_values.h"

#include "base/string.h"

#include <cstdio>
#include <string_view>
#include <vector>

#include <windows.h>

namespace base {

struct LANGCODEPAGE {
  WORD language;
  WORD codepage;
};

static const char* fields[] = {
  "Comments",
  "CompanyName",
  "FileDescription",
  "FileVersion",
  "InternalName",
  "LegalCopyright",
  "LegalTrademarks",
  "OriginalFilename",
  "PrivateBuild",
  "ProductName",
  "ProductVersion",
  "SpecialBuild"
};

std::map<std::string, std::string> ver_query_values(dll lib)
{
  std::string fn = get_dll_filename(lib);
  return ver_query_values(from_utf8(fn).c_str());
}

std::map<std::string, std::string> ver_query_values(const wchar_t* filename)
{
  std::map<std::string, std::string> result;

  DWORD handle;
  DWORD size = GetFileVersionInfoSizeW(filename, &handle);
  if (size == 0)
    return result;

  std::vector<uint8_t> data(size);
  if (!GetFileVersionInfoW(filename, 0, size, &data[0]))
    return result;

  LANGCODEPAGE* pages = nullptr;
  UINT npages = 0;
  VerQueryValueW(&data[0],
                 L"\\VarFileInfo\\Translation",
                 (LPVOID*)&pages,
                 &npages);

  // 41 chars max, e.g.
  //
  //   \StringFileInfo\04091200\OriginalFilename
  //
  std::vector<char> buf(48);

  for (const LANGCODEPAGE
         *p=pages,
         *end=pages + npages/sizeof(LANGCODEPAGE);
       p!=end; ++p) {
    if ((p->language == 0x0409 ||  // US English
         p->language == 0x0809) && // UK English
        (p->codepage == 1200 ||    // Unicode
         p->codepage == 1250 ||    // Latin-2
         p->codepage == 0)) {      // 7-bit ASCII
      for (auto field : fields) {
        if (result.find(field) != result.end())
          continue;

        std::snprintf(&buf[0], buf.size(),
                      "\\StringFileInfo\\%04x%04x\\%s",
                      p->language, p->codepage, field);

        wchar_t* fieldBuf = nullptr;
        UINT fieldLen = 0;
        if (VerQueryValueW(
              &data[0],
              from_utf8(std::string(&buf[0])).c_str(),
              (LPVOID*)&fieldBuf,
              &fieldLen) &&
            fieldBuf &&
            fieldLen) {
          result[field] =
            to_utf8(
              std::wstring(
                std::wstring_view(fieldBuf, fieldLen)));
        }
      }
    }
  }

  return result;
}

} // namespace base
