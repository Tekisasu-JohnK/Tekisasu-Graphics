// LAF Base Library
// Copyright (c) 2019-2020  Igara Studio S.A.
// Copyright (c) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_VERSION_H_INCLUDED
#define BASE_VERSION_H_INCLUDED
#pragma once

#include <string>
#include <vector>

namespace base {

  class Version {
  public:
    typedef std::vector<int> Numbers;

    Version();
    explicit Version(const std::string& from);
    Version(int major, int minor, int patch, int build);

    bool operator<(const Version& other) const;
    bool operator==(const Version& other) const;
    bool operator!=(const Version& other) const { return !operator==(other); }

    const Numbers& numbers() const { return m_numbers; }
    const std::string& prereleaseLabel() const { return m_prereleaseLabel; }
    const int prereleaseNumber() const { return m_prereleaseNumber; }

    std::string str() const;

  private:
    Numbers m_numbers;
    std::string m_prereleaseLabel; // alpha, beta, dev, rc (empty if it's official release)
    int m_prereleaseNumber = 0;
  };

}

#endif
