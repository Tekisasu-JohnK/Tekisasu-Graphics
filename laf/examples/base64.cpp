// LAF Library
// Copyright (c) 2022  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "base/base64.h"
#include "base/buffer.h"
#include "base/file_content.h"
#include "os/os.h"

#include <cstring>

int app_main(int argc, char* argv[])
{
  os::SystemRef system = os::make_system();
  system->setAppMode(os::AppMode::CLI);

  std::string fn;
  bool decode = false;

  for (int i=1; i<argc; ++i) {
    if (std::strcmp(argv[i], "-d") == 0)
      decode = true;
    else
      fn = argv[i];
  }

  base::buffer input;
  if (!fn.empty())
    input = base::read_file_content(fn);
  else
    input = base::read_file_content(stdin);

  base::buffer output;
  if (decode) {
    output = base::decode_base64(input);
    base::set_write_binary_file_content(stdout);
  }
  else {
    std::string tmp = base::encode_base64(input);
    output = base::buffer(tmp.begin(), tmp.end());
  }
  base::write_file_content(stdout, output);
  return 0;
}
