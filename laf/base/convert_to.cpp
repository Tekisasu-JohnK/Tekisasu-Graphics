// LAF Base Library
// Copyright (c) 2023 Igara Studio S.A.
// Copyright (c) 2001-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/convert_to.h"
#include "base/hex.h"
#include "base/sha1.h"
#include "base/uuid.h"

#include <cstdio>
#include <cstdlib>

namespace base {

template<> int convert_to(const std::string& from)
{
  return std::strtol(from.c_str(), NULL, 10);
}

template<> std::string convert_to(const int& from)
{
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%d", from);
  return buf;
}

template<> uint32_t convert_to(const std::string& from)
{
  return std::strtoul(from.c_str(), NULL, 10);
}

template<> std::string convert_to(const uint32_t& from)
{
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%u", from);
  return buf;
}

template<> double convert_to(const std::string& from)
{
  return std::strtod(from.c_str(), NULL);
}

template<> std::string convert_to(const double& from)
{
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%g", from);
  return buf;
}

template<> Sha1 convert_to(const std::string& from)
{
  std::vector<uint8_t> digest(Sha1::HashSize);

  for (size_t i=0; i<Sha1::HashSize; ++i) {
    if (i*2+1 >= from.size())
      break;

    digest[i] = convert_to<int>(from.substr(i*2, 2));
  }

  return Sha1(digest);
}

template<> std::string convert_to(const Sha1& from)
{
  char buf[3];
  std::string res;
  res.reserve(2*Sha1::HashSize);

  for(int c=0; c<Sha1::HashSize; ++c) {
    snprintf(buf, sizeof(buf), "%02x", from[c]);
    res += buf;
  }

  return res;
}

template<> Uuid convert_to(const std::string& from)
{
  Uuid uuid;
  int i = 0;
  for (int j=0; j<int(from.size()) && i<16; ) {
    int a = hex_to_int(from[j++]);
    int b = hex_to_int(from[j++]);
    (uuid.bytes())[i++] = ((a << 4) | b);

    // Skip
    if (i == 4 || i == 6 || i == 8 || i == 10) {
      if (from[j] == '-')
        ++j;
      else
        return Uuid();
    }
  }
  return uuid;
}

template<> std::string convert_to(const Uuid& from)
{
  int i = 0;
  char buf[Uuid::HashSize+1];
  for (; i<4; ++i) snprintf(buf+2*i, 3, "%02x", int(from[i]));
  for (; i<6; ++i) snprintf(buf+2*i+1, 3, "%02x", int(from[i]));
  for (; i<8; ++i) snprintf(buf+2*i+2, 3, "%02x", int(from[i]));
  for (; i<10; ++i) snprintf(buf+2*i+3, 3, "%02x", int(from[i]));
  for (; i<16; ++i) snprintf(buf+2*i+4, 3, "%02x", int(from[i]));
  buf[8] = buf[13] = buf[18] = buf[23] = '-';
  return std::string(buf);
}

} // namespace base
