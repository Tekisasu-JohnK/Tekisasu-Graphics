// LAF OS Library
// Copyright (C) 2024  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include "base/fs.h"
#include "os/window.h"
#include "os/win/dnd.h"
#include "os/system.h"

#include "clip/clip.h"
#include "clip/clip_win.h"

#include <shlobj.h>

namespace {

DWORD as_dropeffect(const os::DropOperation op)
{
  DWORD effect = DROPEFFECT_NONE;
  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Copy))
    effect |= DROPEFFECT_COPY;

  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Move))
    effect |= DROPEFFECT_MOVE;

  if (static_cast<int>(op) & static_cast<int>(os::DropOperation::Link))
    effect |= DROPEFFECT_LINK;

  return effect;
}

os::DropOperation as_dropoperation(DWORD pdwEffect)
{
  int op = 0;
  if (pdwEffect & DROPEFFECT_COPY)
    op |= static_cast<int>(os::DropOperation::Copy);

  if (pdwEffect & DROPEFFECT_MOVE)
    op |= static_cast<int>(os::DropOperation::Move);

  if (pdwEffect & DROPEFFECT_LINK)
    op |= static_cast<int>(os::DropOperation::Link);

  return static_cast<os::DropOperation>(op);
}

// HGLOBAL Locking/Unlocking wrapper
template<typename T>
class GLock {
public:
  GLock() = delete;
  GLock(const GLock&) = delete;
  explicit GLock(HGLOBAL hglobal) : m_hmem(hglobal) {
    m_data = static_cast<T>(GlobalLock(m_hmem));
  }

  virtual ~GLock() { GlobalUnlock(m_hmem); }

  operator HGLOBAL() { return m_hmem; }

  operator T() { return m_data; }

  T operator->() { return m_data; }

  bool operator==(std::nullptr_t) const { return m_data == nullptr; }
  bool operator!=(std::nullptr_t) const { return m_data != nullptr; }

  SIZE_T size() { return GlobalSize(m_hmem); }

private:
  HGLOBAL m_hmem = nullptr;
  T m_data = nullptr;
};

// STGMEDIUM wrapper. This is a specific wrapper for the case when the medium
// is of TYMED_HGLOBAL type. Maybe it could be generalized to support the
// other types of mediums, but this is not needed right now, so I'm leaving
// this as is.
template<typename T>
class Medium : public GLock<T> {
public:
  Medium() = delete;
  Medium(const Medium&) = delete;
  Medium(std::nullptr_t) : GLock<T>(nullptr) {
    std::memset(&m_medium, 0, sizeof(STGMEDIUM));
  }
  explicit Medium(const STGMEDIUM& medium) : GLock<T>(medium.hGlobal) {
    m_medium = medium;
  }

  ~Medium() override { ReleaseStgMedium(&m_medium); }

private:
  STGMEDIUM m_medium;
};

// IDataObject wrapper.
class DataWrapper {
public:
  DataWrapper() = delete;
  DataWrapper(const DataWrapper&) = delete;
  DataWrapper(IDataObject* data) : m_data(data) {}

  template<typename T>
  Medium<T> get(CLIPFORMAT cfmt, LONG lindex = -1)
  {
    STGMEDIUM medium;
    FORMATETC fmt;
    fmt.cfFormat = cfmt;
    fmt.ptd = nullptr;
    fmt.dwAspect = DVASPECT_CONTENT;
    fmt.lindex = lindex;
    fmt.tymed = TYMED::TYMED_HGLOBAL;
    if (m_data->GetData(&fmt, &medium) != S_OK)
      return nullptr;

    return Medium<T>(medium);
  }

private:
  IDataObject* m_data = nullptr;
};

} // anonymous namespace

namespace os {

#if CLIP_ENABLE_IMAGE
SurfaceRef default_decode_png(const uint8_t* buf, uint32_t len)
{
  clip::image img;
  if (!clip::win::read_png(buf, len, &img, nullptr))
    return nullptr;

  return os::instance()->makeSurface(img);
}

SurfaceRef default_decode_jpg(const uint8_t* buf, uint32_t len)
{
  clip::image img;
  if (!clip::win::read_jpg(buf, len, &img, nullptr))
    return nullptr;

  return os::instance()->makeSurface(img);
}

SurfaceRef default_decode_bmp(const uint8_t* buf, uint32_t len)
{
  clip::image img;
  if (!clip::win::read_bmp(buf, len, &img, nullptr))
    return nullptr;

  return os::instance()->makeSurface(img);
}

SurfaceRef default_decode_gif(const uint8_t* buf, uint32_t len)
{
  clip::image img;
  if (!clip::win::read_gif(buf, len, &img, nullptr))
    return nullptr;

  return os::instance()->makeSurface(img);
}
#endif

base::paths DragDataProviderWin::getPaths()
{
  base::paths files;
  DataWrapper data(m_data);
  Medium<HDROP> hdrop = data.get<HDROP>(CF_HDROP);
  if (hdrop != nullptr) {
    int count = DragQueryFile(hdrop, 0xFFFFFFFF, nullptr, 0);
    for (int index = 0; index < count; ++index) {
      int length = DragQueryFile(hdrop, index, nullptr, 0);
      if (length > 0) {
        // From Win32 docs: https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-dragqueryfilew
        // the DragQueryFile() doesn't include the null character in its return value.
        std::vector<TCHAR> str(length + 1);
        DragQueryFile(hdrop, index, str.data(), str.size());
        files.push_back(base::to_utf8(str.data()));
      }
    }
  }
  return files;
}

#if CLIP_ENABLE_IMAGE
SurfaceRef DragDataProviderWin::getImage()
{
  SurfaceRef surface = nullptr;

  DataWrapper data(m_data);
  UINT png_format = RegisterClipboardFormatA("PNG");
  if (png_format) {
    Medium<uint8_t*> png_handle = data.get<uint8_t*>(png_format);
    if (png_handle != nullptr)
      return os::decode_png(png_handle, png_handle.size());
  }

  clip::image img;
  Medium<BITMAPV5HEADER*> b5 = data.get<BITMAPV5HEADER*>(CF_DIBV5);
  if (b5 != nullptr) {
    clip::win::BitmapInfo bi(b5);
    if (bi.to_image(img))
      return os::instance()->makeSurface(img);
  }

  Medium<BITMAPINFO*> hbi = data.get<BITMAPINFO*>(CF_DIB);
  if (hbi != nullptr) {
    clip::win::BitmapInfo bi(hbi);
    if (bi.to_image(img))
      return os::instance()->makeSurface(img);
  }

  // If there is a file descriptor available, then we inspect the first
  // file of the group to see if its filename has any of the supported
  // filename extensions for image formats.
  UINT fileDescriptorFormat = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
  UINT fileContentsFormat = RegisterClipboardFormat(CFSTR_FILECONTENTS);
  if (fileDescriptorFormat) {
    Medium<FILEGROUPDESCRIPTOR*> fgd = data.get<FILEGROUPDESCRIPTOR*>(fileDescriptorFormat);
    if (fgd != nullptr &&  fgd->cItems > 0) {
      // Get content of the first file on the group.
      Medium<uint8_t*> content = data.get<uint8_t*>(fileContentsFormat, 0);
      if (content != nullptr) {
        std::string filename(base::to_utf8(fgd->fgd->cFileName));
        std::string ext = base::get_file_extension(filename);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

        if (ext == "PNG")
          return os::decode_png(content, content.size());

        if (ext == "JPG" || ext == "JPEG" || ext == "JPE")
          return os::decode_jpg(content, content.size());

        if (ext == "GIF")
          return os::decode_gif(content, content.size());

        if (ext == "BMP")
          return os::decode_bmp(content, content.size());
      }
    }
  }

  // No suitable image format found.
  return nullptr;
}
#endif

std::string DragDataProviderWin::getUrl()
{
  DataWrapper data(m_data);
  UINT urlFormat = RegisterClipboardFormat(CFSTR_INETURL);

  Medium<TCHAR*> url = data.get<TCHAR*>(urlFormat);
  if (url == nullptr)
    return std::string();

  return std::string(base::to_utf8((TCHAR*)url));
}

bool DragDataProviderWin::contains(DragDataItemType type)
{
  base::ComPtr<IEnumFORMATETC> formats;
  if (m_data->EnumFormatEtc(DATADIR::DATADIR_GET, &formats) != S_OK)
    return false;

  UINT urlFormat = RegisterClipboardFormat(CFSTR_INETURL);
  UINT pngFormat = RegisterClipboardFormat(L"PNG");
  UINT fileDescriptorFormat = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
  char name[101];
  FORMATETC fmt;
  while (formats->Next(1, &fmt, nullptr) == S_OK) {
    switch (fmt.cfFormat) {
      case CF_HDROP:
        if (type == DragDataItemType::Paths)
          return true;
        break;
#if CLIP_ENABLE_IMAGE
      case CF_DIBV5:
        if (type == DragDataItemType::Image)
          return true;
        break;
      case CF_DIB:
        if (type == DragDataItemType::Image)
          return true;
        break;
#endif
      default: {
        switch (type) {
#if CLIP_ENABLE_IMAGE
          case DragDataItemType::Image:
            if (fmt.cfFormat == pngFormat)
              return true;

            if (fmt.cfFormat == fileDescriptorFormat) {
              DataWrapper data(m_data);
              Medium<FILEGROUPDESCRIPTOR*> fgd = data.get<FILEGROUPDESCRIPTOR*>(fileDescriptorFormat);
              if (fgd != nullptr && fgd->cItems > 0) {
                const std::string filename(base::to_utf8(fgd->fgd->cFileName));
                std::string ext = base::get_file_extension(filename);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
                if (ext == "PNG" || ext == "JPG" || ext == "JPEG" ||
                    ext == "JPE" || ext == "GIF" || ext == "BMP")
                  return true;
              }
            }
            break;
#endif
          case DragDataItemType::Url:
            if (fmt.cfFormat == urlFormat)
              return true;
            break;
        }
        break;
      }
    }
  }
  return false;
}

STDMETHODIMP DragTargetAdapter::QueryInterface(REFIID riid, LPVOID* ppv)
{
  if (!ppv)
    return E_INVALIDARG;

  *ppv = nullptr;
  if (riid != IID_IDropTarget && riid != IID_IUnknown)
    return E_NOINTERFACE;

  *ppv = static_cast<IDropTarget*>(this);
  AddRef();
  return NOERROR;
}

ULONG DragTargetAdapter::AddRef()
{
  return InterlockedIncrement(&m_ref);
}

ULONG DragTargetAdapter::Release()
{
  // Decrement the object's internal counter.
  ULONG ref = InterlockedDecrement(&m_ref);
  if (0 == ref)
    delete this;

  return ref;
}

DragEvent DragTargetAdapter::newDragEvent(POINTL* pt, DWORD* pdwEffect)
{
  if (pt)
    // Get drag position
    m_position = m_window->pointFromScreen(gfx::Point(pt->x, pt->y));

  std::unique_ptr<DragDataProvider> ddProvider = std::make_unique<DragDataProviderWin>(m_data.get());
  return DragEvent(m_window,
                   as_dropoperation(*pdwEffect),
                   m_position,
                   ddProvider);
}

STDMETHODIMP DragTargetAdapter::DragEnter(IDataObject* pDataObj,
                                          DWORD grfKeyState,
                                          POINTL pt,
                                          DWORD* pdwEffect)
{
  if (!m_window->hasDragTarget())
    return E_NOTIMPL;

  m_data = base::ComPtr<IDataObject>(pDataObj);
  if (!m_data)
    return E_UNEXPECTED;

  DragEvent ev = newDragEvent(&pt, pdwEffect);
  m_window->notifyDragEnter(ev);
  *pdwEffect = as_dropeffect(ev.dropResult());
  return S_OK;
}

STDMETHODIMP DragTargetAdapter::DragOver(DWORD grfKeyState,
                                         POINTL pt,
                                         DWORD* pdwEffect)
{
  if (!m_window->hasDragTarget())
    return E_NOTIMPL;

  DragEvent ev = newDragEvent(&pt, pdwEffect);
  m_window->notifyDrag(ev);
  *pdwEffect = as_dropeffect(ev.dropResult());
  return S_OK;
}

STDMETHODIMP DragTargetAdapter::DragLeave(void)
{
  if (!m_window->hasDragTarget())
    return E_NOTIMPL;

  DragEvent ev = newDragEvent(nullptr, DROPEFFECT_NONE);
  m_window->notifyDragLeave(ev);
  m_data.reset();
  return S_OK;
}

STDMETHODIMP DragTargetAdapter::Drop(IDataObject* pDataObj,
                                     DWORD grfKeyState,
                                     POINTL pt,
                                     DWORD* pdwEffect)
{
  if (!m_window->hasDragTarget())
    return E_NOTIMPL;

  m_data = base::ComPtr<IDataObject>(pDataObj);
  if (!m_data)
    return E_UNEXPECTED;

  DragEvent ev = newDragEvent(&pt, pdwEffect);
  m_window->notifyDrop(ev);
  m_data = nullptr;
  *pdwEffect = as_dropeffect(ev.dropResult());
  return S_OK;
}

} // namespase os
