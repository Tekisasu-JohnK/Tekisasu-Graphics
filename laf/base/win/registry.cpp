// LAF Base Library
// Copyright (c) 2017 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/win/registry.h"

#include "base/string.h"
#include "base/win/win32_exception.h"

#include <string>
#include <vector>

namespace base {

namespace {

REGSAM to_regsam(const hkey::access a)
{
  switch (a) {
    case hkey::access::read: return KEY_READ;
    case hkey::access::write: return KEY_READ | KEY_WRITE;
  }
  return KEY_ALL_ACCESS;
}

} // anonymous namespace

hkey hkey::classes_root()   { return hkey(HKEY_CLASSES_ROOT); }
hkey hkey::current_config() { return hkey(HKEY_CURRENT_CONFIG); }
hkey hkey::current_user()   { return hkey(HKEY_CURRENT_USER); }
hkey hkey::local_machine()  { return hkey(HKEY_LOCAL_MACHINE); }
hkey hkey::users()          { return hkey(HKEY_USERS); }

hkey::hkey(HKEY hkey)
  : m_hkey(hkey)
{
}

hkey::hkey(hkey&& key)
  : m_hkey(nullptr)
{
  std::swap(m_hkey, key.m_hkey);
}

hkey::~hkey()
{
  close();
}

hkey& hkey::operator=(hkey&& key)
{
  close();
  std::swap(m_hkey, key.m_hkey);
  return *this;
}

hkey hkey::open(const std::string& subkey, const access a)
{
  if (!m_hkey)
    return hkey(nullptr);

  HKEY openKey = nullptr;
  LONG result = RegOpenKeyExW(m_hkey, from_utf8(subkey).c_str(),
                              0, to_regsam(a), &openKey);
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error opening registry key");

  return hkey(openKey);
}

hkey hkey::create(const std::string& subkey)
{
  if (!m_hkey)
    return nullptr;

  HKEY newKey = nullptr;
  LONG result = RegCreateKeyExW(m_hkey, from_utf8(subkey).c_str(),
                                0, nullptr,
                                REG_OPTION_NON_VOLATILE,
                                KEY_READ | KEY_WRITE,
                                nullptr, &newKey, nullptr);
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error creating registry key");

  return hkey(newKey);
}

void hkey::close()
{
  if (m_hkey) {
    // If m_key is a predefined HKEY like HKEY_CLASSES_ROOT, etc. this
    // does nothing anyway.
    RegCloseKey(m_hkey);
    m_hkey = nullptr;
  }
}

std::string hkey::string(const std::string& name)
{
  if (!m_hkey)
    return std::string();

  std::wstring wname = from_utf8(name);

  DWORD type = 0, reqSize = 0;
  LONG result = RegQueryValueExW(m_hkey, wname.c_str(), nullptr,
                                 (LPDWORD)&type, nullptr,
                                 (LPDWORD)&reqSize);

  // Value name not found in this key
  if (result == ERROR_FILE_NOT_FOUND)
    return std::string();

  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error getting registry value size");

  if (reqSize == 0 || type != REG_SZ)
    return std::string();

  std::vector<wchar_t> buf(reqSize / sizeof(wchar_t) + 1);
  result = RegQueryValueExW(m_hkey, wname.c_str(), nullptr,
                            nullptr, (LPBYTE)&buf[0],
                            (LPDWORD)&reqSize);
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error getting registry value");

  buf[buf.size()-1] = 0;

  return to_utf8(&buf[0]);
}

void hkey::string(const std::string& name, const std::string& value)
{
  if (!m_hkey)
    return;

  std::wstring wvalue = from_utf8(value);
  LONG result = RegSetValueExW(m_hkey, from_utf8(name).c_str(),
                               0, REG_SZ,
                               (LPBYTE)wvalue.c_str(),
                               (DWORD)(wvalue.size()+1)*sizeof(wchar_t));
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error setting registry value");
}

DWORD hkey::dword(const std::string& name)
{
  if (!m_hkey)
    return 0;

  DWORD
    type = 0,
    dword = 0,
    size = sizeof(DWORD);
  LONG result = RegQueryValueExW(m_hkey, from_utf8(name).c_str(), nullptr,
                                 (LPDWORD)&type, (LPBYTE)&dword,
                                 (LPDWORD)&size);

  // Value name not found in this key
  if (result == ERROR_FILE_NOT_FOUND)
    return 0;

  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error getting registry value");

  if (type != REG_DWORD)
    return 0;

  return dword;
}

void hkey::dword(const std::string& name, const DWORD value)
{
  if (!m_hkey)
    return;

  LONG result = RegSetValueExW(m_hkey, from_utf8(name).c_str(),
                               0, REG_DWORD,
                               (LPBYTE)&value,
                               (DWORD)sizeof(DWORD));
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error setting registry value");
}

void hkey::delete_tree(const std::string& subkey)
{
  if (!m_hkey)
    return;

  LONG result = RegDeleteTreeW(m_hkey, from_utf8(subkey).c_str());
  if (result != ERROR_SUCCESS)
    throw Win32Exception("Error deleting registry key");
}

} // namespace base
