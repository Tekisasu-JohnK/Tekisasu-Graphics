// LAF Base Library
// Copyright (c) 2022 Igara Studio S.A.
// Copyright (c) 2015-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "base/base64.h"
#include "base/debug.h"
#include "base/ints.h"

#include <cmath>

namespace base {

static const char base64Table[] = "ABCDEFGHIJKLMNOP"
                                  "QRSTUVWXYZabcdef"
                                  "ghijklmnopqrstuv"
                                  "wxyz0123456789+/";

static const int invBase64Table[] = {
 // From 32 to 47                 '+'         '/'
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
 // From 48 to 63     '9'
 52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0,
 // From 64 to 79
  0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
 // From 80 to 95
 15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
 // From 96 to 111                            'o'
  0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
 // From 112 to 122            'z'
 41,42,43,44,45,46,47,48,49,50,51 };

static inline char base64Char(int index)
{
  ASSERT(index >= 0 && index < 64);
  return base64Table[index];
}

static inline int base64Inv(int asciiChar)
{
  asciiChar -= 32;
  if (asciiChar >= 0 && asciiChar < sizeof(invBase64Table))
    return invBase64Table[asciiChar];
  else
    return 0;
}

void encode_base64(const char* input, size_t n, std::string& output)
{
  size_t size = 4*int(std::ceil(n/3.0)); // Estimate encoded string size
  output.resize(size);

  auto outIt = output.begin();
  auto outEnd = output.end();
  uint8_t next = 0;
  size_t i = 0;
  size_t j = 0;
  for (; i<n; ++i, ++input) {
    auto inputValue = *input;
    switch (j) {
      case 0:
        *outIt = base64Char((inputValue & 0b11111100) >> 2);
        ++outIt;
        next |= (inputValue & 0b00000011) << 4;
        ++j;
        break;
      case 1:
        *outIt = base64Char(((inputValue & 0b11110000) >> 4) | next);
        ++outIt;
        next = (inputValue & 0b00001111) << 2;
        ++j;
        break;
      case 2:
        *outIt = base64Char(((inputValue & 0b11000000) >> 6) | next);
        ++outIt;
        *outIt = base64Char(inputValue & 0b00111111);
        ++outIt;
        next = 0;
        j = 0;
        break;
    }
  }

  if (outIt != outEnd) {
    if (next) {
      *outIt = base64Char(next);
      ++outIt;
    }
    for (; outIt!=outEnd; ++outIt)
      *outIt = '=';            // Padding
  }
}

void decode_base64(const char* input, size_t n, buffer& output)
{
  size_t size = 3*int(std::ceil(n/4.0)); // Estimate decoded buffer size
  output.resize(size);

  auto outIt = output.begin();
  auto outEnd = output.end();
  size_t i = 0;
  for (; i+3<n; i+=4, input+=4) {
    *outIt = (((base64Inv(input[0])           ) << 2) |
              ((base64Inv(input[1]) & 0b110000) >> 4));
    ++outIt;

    if (input[2] == '=') {
      size -= 2;
      break;
    }

    *outIt = (((base64Inv(input[1]) & 0b001111) << 4) |
              ((base64Inv(input[2]) & 0b111100) >> 2));
    ++outIt;

    if (input[3] == '=') {
      --size;
      break;
    }

    *outIt = (((base64Inv(input[2]) & 0b000011) << 6) |
              ((base64Inv(input[3])           )));
    ++outIt;
  }

  if (output.size() > size)
    output.resize(size);
}

void decode_base64(const char* input, size_t n, std::string& output)
{
  buffer tmp;
  decode_base64(input, n, tmp);
  output = std::string((const char*)&tmp[0], tmp.size());
}

} // namespace base
