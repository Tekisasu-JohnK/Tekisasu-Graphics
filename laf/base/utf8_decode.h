// LAF Base Library
// Copyright (c) 2022 Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_UTF8_DECODE_H_INCLUDED
#define BASE_UTF8_DECODE_H_INCLUDED
#pragma once

#include <string>

namespace base {

  class utf8_decode {
  public:
    using string = std::string;
    using string_ref = const std::string&;
    using iterator = std::string::const_iterator;

    utf8_decode() { }
    utf8_decode(const utf8_decode&) = default;
    utf8_decode& operator=(const utf8_decode&) = default;

    explicit utf8_decode(string_ref str)
      : m_it(str.begin())
      , m_end(str.end()) {
    }

    iterator pos() const {
      return m_it;
    }

    bool is_end() const {
      return m_it == m_end;
    }

    bool is_valid() const {
      return m_valid;
    }

    int next() {
      if (m_it == m_end)
        return 0;

      int c = *m_it;
      ++m_it;

      // UTF-8 escape bit 0x80 to encode larger code points
      if (c & 0b1000'0000) {
        // Get the number of bits following the first one 0b1xxx'xxxx,
        // which indicates the number of extra bytes in the input
        // string following this one, and that will be part of the
        // final Unicode code point.
        //
        // This is like "number of leading ones", similar to a
        // __builtin_clz(~x)-24 (for 8 bits), anyway doing some tests,
        // the CLZ intrinsic is not faster than this code in x86_64.
        int n = 0;
        int f = 0b0100'0000;
        while (c & f) {
          ++n;
          f >>= 1;
        }

        if (n == 0) {
          // Invalid UTF-8: 0b10xx'xxxx alone, i.e. not inside a
          // escaped sequence (e.g. after 0b110xx'xxx
          m_valid = false;
          return 0;
        }

        // Keep only the few initial data bits from the first byte (6
        // first bits if we have only one extra char, then for each
        // extra char we have less useful data in this first byte).
        c &= (0b0001'1111 >> (n-1));

        while (n--) {
          if (m_it == m_end) {
            // Invalid UTF-8: missing 0b10xx'xxxx bytes
            m_valid = false;
            return 0;
          }
          const int chr = *m_it;
          ++m_it;
          if ((chr & 0b1100'0000) != 0b1000'0000) {
            // Invalid UTF-8: Extra byte doesn't contain 0b10xx'xxxx
            m_valid = false;
            return 0;
          }
          // Each extra byte in the encoded string adds 6 bits of
          // information for the final Unicode code point.
          c = (c << 6) | (chr & 0b0011'1111);
        }
      }

      return c;
    }

  private:
    iterator m_it;
    iterator m_end;
    bool m_valid = true;
  };

}

#endif
