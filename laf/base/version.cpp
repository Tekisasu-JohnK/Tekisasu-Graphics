// LAF Base Library
// Copyright (c) 2019-2021  Igara Studio S.A.
// Copyright (c) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/version.h"

#include "base/convert_to.h"

#include <cctype>

namespace base {

Version::Version()
{
}

Version::Version(const std::string& from)
{
  std::string::size_type i = 0;
  std::string::size_type j = 0;
  std::string::size_type k = from.find('-', 0);

  while ((j != std::string::npos) &&
         (k == std::string::npos || j < k)) {
    j = from.find('.', i);

    std::string number;
    if (j != std::string::npos) {
      number = from.substr(i, j - i);
      i = j+1;
    }
    else {
      if (from.size() == i) // Empty string
        break;

      number = from.substr(i);
    }

    m_numbers.push_back(convert_to<int>(number));
  }

  if (k != std::string::npos) {
    auto k0 = ++k;
    for (; k < from.size() && !std::isdigit(from[k]); ++k)
      ;
    if (k < from.size()) {
      m_prereleaseNumber = convert_to<int>(from.substr(k));
      m_prereleaseLabel = from.substr(k0, k - k0);
    }
    else
      m_prereleaseLabel = from.substr(k0);
  }

  while (!m_prereleaseLabel.empty() &&
         m_prereleaseLabel[m_prereleaseLabel.size()-1] == '.')
    m_prereleaseLabel.erase(m_prereleaseLabel.size()-1);
}

Version::Version(int major, int minor, int patch, int build)
{
  m_numbers.push_back(major);
  if (minor || patch || build)
    m_numbers.push_back(minor);
  if (patch || build)
    m_numbers.push_back(patch);
  if (build)
    m_numbers.push_back(build);
}

bool Version::operator<(const Version& other) const
{
  Numbers::const_iterator
    it1 = m_numbers.begin(), end1 = m_numbers.end(),
    it2 = other.m_numbers.begin(), end2 = other.m_numbers.end();

  while (it1 != end1 || it2 != end2) {
    int number1 = (it1 != end1 ? *it1++: 0);
    int number2 = (it2 != end2 ? *it2++: 0);

    if (number1 < number2)
      return true;
    else if (number1 > number2)
      return false;
  }

  if (m_prereleaseLabel.empty()) {
    return false;
  }
  else if (other.m_prereleaseLabel.empty()) {
    return true;
  }
  else {
    int res = m_prereleaseLabel.compare(other.m_prereleaseLabel);
    if (res < 0)
      return true;
    else if (res > 0)
      return false;
    else
      return (m_prereleaseNumber < other.m_prereleaseNumber);
  }

  return false;
}

bool Version::operator==(const Version& other) const
{
  return (m_numbers == other.m_numbers &&
          m_prereleaseLabel == other.m_prereleaseLabel);
}

std::string Version::str() const
{
  std::string res;
  for (auto number : m_numbers) {
    if (!res.empty())
      res.push_back('.');
    res += base::convert_to<std::string>(number);
  }
  if (!m_prereleaseLabel.empty()) {
    res.push_back('-');
    res += m_prereleaseLabel;
    if (m_prereleaseNumber)
      res += base::convert_to<std::string>(m_prereleaseNumber);
  }
  return res;
}

} // namespace base
