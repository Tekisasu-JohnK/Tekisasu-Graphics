// LAF OS Library
// Copyright (c) 2019  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef OS_SKIA_SKIA_FONT_MANAGER_INCLUDED
#define OS_SKIA_SKIA_FONT_MANAGER_INCLUDED
#pragma once

#include "os/font_manager.h"

#include "os/font_style.h"

#include "SkFontMgr.h"
#include "SkString.h"
#include "SkTypeface.h"

namespace os {

class SkiaTypeface : public Typeface {
public:
  SkiaTypeface(SkTypeface* skTypeface)
    : m_skTypeface(skTypeface) {
  }

  void dispose() override {
    delete this;
  }

  FontStyle fontStyle() const override {
    SkFontStyle skStyle = m_skTypeface->fontStyle();
    return FontStyle((FontStyle::Weight)skStyle.weight(),
                     (FontStyle::Width)skStyle.width(),
                     (FontStyle::Slant)skStyle.slant());
  }

private:
  sk_sp<SkTypeface> m_skTypeface;
};

class SkiaFontStyleSet : public FontStyleSet {
public:
  SkiaFontStyleSet(SkFontStyleSet* set)
    : m_skSet(set) {
  }

  void dispose() override {
    delete this;
  }

  int count() override {
    return m_skSet->count();
  }

  void getStyle(int index,
                FontStyle& style,
                std::string& name) override {
    SkFontStyle skStyle;
    SkString skName;
    m_skSet->getStyle(index, &skStyle, &skName);
    style = FontStyle((FontStyle::Weight)skStyle.weight(),
                      (FontStyle::Width)skStyle.width(),
                      (FontStyle::Slant)skStyle.slant());
    name = skName.c_str();
  }

  TypefaceHandle typeface(int index) override {
    return new SkiaTypeface(m_skSet->createTypeface(index));
  }

  TypefaceHandle matchStyle(const FontStyle& style) override {
    SkFontStyle skStyle((SkFontStyle::Weight)style.weight(),
                        (SkFontStyle::Width)style.width(),
                        (SkFontStyle::Slant)style.slant());
    return new SkiaTypeface(m_skSet->matchStyle(skStyle));
  }

private:
  sk_sp<SkFontStyleSet> m_skSet;
};

class SkiaFontManager : public FontManager {
public:
  SkiaFontManager()
    : m_skFontMgr(SkFontMgr::RefDefault()) {
  }

  ~SkiaFontManager() {
  }

  int countFamilies() const override {
    return m_skFontMgr->countFamilies();
  }

  std::string familyName(int i) const override {
    SkString name;
    m_skFontMgr->getFamilyName(i, &name);
    return std::string(name.c_str());
  }

  FontStyleSetHandle familyStyleSet(int i) const override {
    return new SkiaFontStyleSet(m_skFontMgr->createStyleSet(i));
  }

  FontStyleSetHandle matchFamily(const std::string& familyName) const override {
    return new SkiaFontStyleSet(m_skFontMgr->matchFamily(familyName.c_str()));
  }

private:
  sk_sp<SkFontMgr> m_skFontMgr;
};

} // namespace os

#endif
