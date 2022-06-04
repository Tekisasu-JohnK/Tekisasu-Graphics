// LAF Base Library
// Copyright (c) 2021 Igara Studio S.A.
// Copyright (c) 2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_WIN_REGISTRY_H_INCLUDED
#define BASE_WIN_REGISTRY_H_INCLUDED

#if !LAF_WINDOWS
  #error This header file can be used only on Windows platform
#endif

#include "base/disable_copying.h"

#include <string>
#include <windows.h>

namespace base {

class hkey {
public:
  enum access {
    read = 1,
    write = 2,
  };

  static hkey classes_root();
  static hkey current_config();
  static hkey current_user();
  static hkey local_machine();
  static hkey users();

  hkey(HKEY hkey);
  hkey(hkey&& key);
  ~hkey();

  hkey& operator=(hkey&& key);

  HKEY handle() const { return m_hkey; }

  hkey open(const std::string& subkey, const access a);
  hkey create(const std::string& subkey);
  void close();

  std::string string(const std::string& name);
  void string(const std::string& name, const std::string& value);

  DWORD dword(const std::string& name);
  void dword(const std::string& name, const DWORD value);

  // Deletes keys in the given subkey, if subkey == "", all children
  // keys are deleted but this hkey isn't (the root is kept).
  void delete_tree(const std::string& subkey);

private:
  HKEY m_hkey;

  DISABLE_COPYING(hkey);
};

} // namespace base

#endif
