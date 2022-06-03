// Aseprite Code Generator
// Copyright (c) 2014-2016 David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef GEN_UI_CLASS_H_INCLUDED
#define GEN_UI_CLASS_H_INCLUDED
#pragma once

#include <string>
#include "tinyxml.h"

void gen_ui_class(TiXmlDocument* doc,
                  const std::string& inputFn,
                  const std::string& widgetId);

#endif
