// LAF Base Library
// Copyright (c) 2020  Igara Studio S.A.
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_REF_H_INCLUDED
#define BASE_REF_H_INCLUDED
#pragma once

#include "base/debug.h"
#include "base/ints.h"

#include <atomic>

namespace base {

  template<typename T>
  class RefCountT {
  public:
    RefCountT() : m_ref(1) { }

    ~RefCountT() {
      // m_ref can be 1 in case that RefCountT() was created in the
      // stack, and 0 when it's deleted from unref().
      ASSERT(m_ref == 0 || m_ref == 1);
    }

    RefCountT(const RefCountT&) : m_ref(1) { }
    RefCountT& operator=(const RefCountT&) { return *this; }

    void ref() {
      ASSERT(m_ref > 0);
      m_ref.fetch_add(1, std::memory_order_relaxed);
    }

    void unref() {
      ASSERT(m_ref > 0);
      if (m_ref.fetch_sub(1, std::memory_order_acq_rel) == 1)
        delete (T*)this;
    }

#ifdef _DEBUG   // For debugging purposes only (TRACE, TRACEARGS, etc.)
    uint32_t ref_count() const { return m_ref; }
#endif

  private:
    std::atomic<uint32_t> m_ref;
  };

  class RefCount : public RefCountT<RefCount> {
  public:
    RefCount() { }
    virtual ~RefCount() { }

    // Copy and move disabled for classes with virtual destruction
    RefCount(const RefCount&) = delete;
    RefCount(RefCount&&) = delete;
    RefCount& operator=(const RefCount&) = delete;
    RefCount& operator=(RefCount&&) = delete;
  };

  // Smart pointer for RefCountT objects
  template<typename T>
  class Ref {
  public:
    Ref() noexcept : m_ptr(nullptr) { }
    Ref(std::nullptr_t) noexcept : m_ptr(nullptr) { }

    explicit Ref(T* ptr) noexcept : m_ptr(ptr) { }
    template<typename U>
    explicit Ref(U* ptr) noexcept : m_ptr(static_cast<T*>(ptr)) { }

    Ref(Ref<T>&& ref) noexcept : m_ptr(ref.release()) { }
    template<typename U>
    Ref(Ref<U>&& ref) noexcept : m_ptr(static_cast<T*>(ref.release())) { }

    Ref(const Ref<T>& ref) : m_ptr(ref.m_ptr) {
      if (m_ptr) m_ptr->ref();
    }
    template<typename U>
    Ref(const Ref<U>& ref) : m_ptr(static_cast<T*>(ref.m_ptr)) {
      if (m_ptr) m_ptr->ref();
    }

    ~Ref() { if (m_ptr) m_ptr->unref(); }

    void reset(T* ptr = nullptr) {
      if (m_ptr) m_ptr->unref();
      m_ptr = ptr;
      if (m_ptr) m_ptr->ref();
    }

    T* release() {
      T* ptr = m_ptr;
      m_ptr = nullptr;
      return ptr;
    }

    T* get() const { return m_ptr; }
    T* operator->() const { return m_ptr; }
    T& operator*() const { return *m_ptr; }

    // Do not define operator=(T*) because stealing references from
    // raw pointers is like an implicit copy ctor from a raw pointer.
    // Use AddRef() for these cases, e.g.:
    //
    // void function(T* refCountedObj) {
    //   Ref<T> p;
    //   p = AddRef(refCountedObj);
    //   ...
    // }
    Ref<T>& operator=(T* ptr) = delete;

    Ref<T>& operator=(std::nullptr_t) {
      reset();
      return *this;
    }

    Ref<T>& operator=(Ref<T>&& ref) {
      if (m_ptr) m_ptr->unref();
      m_ptr = ref.release();
      return *this;
    }

    Ref<T>& operator=(const Ref<T>& ref) {
      if (m_ptr) {
        if (m_ptr == ref.m_ptr)
          return *this;
        m_ptr->unref();
      }
      m_ptr = ref.m_ptr;
      if (m_ptr) m_ptr->ref();
      return *this;
    }

    // Comparison between other Ref, raw pointers and nullpointers
    bool operator==(const Ref<T>& r) const { return m_ptr == r.m_ptr; }
    bool operator!=(const Ref<T>& r) const { return m_ptr != r.m_ptr; }
    bool operator==(const T* p) const { return m_ptr == p; }
    bool operator!=(const T* p) const { return m_ptr != p; }
    bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
    bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }

    explicit operator bool() const {
      return m_ptr != nullptr;
    }

    void swap(Ref<T>& r) noexcept {
      std::swap(m_ptr, r.m_ptr);
    }

  private:
    T* m_ptr;
  };

  template<typename T>
  inline bool operator==(T* ptr, const Ref<T>& r) {
    return r.get() == ptr;
  }

  template<typename T>
  inline bool operator!=(T* ptr, const Ref<T>& r) {
    return r.get() != ptr;
  }

  template<typename T>
  inline bool operator==(std::nullptr_t, const Ref<T>& r) {
    return r.get() == nullptr;
  }

  template<typename T>
  inline bool operator!=(std::nullptr_t, const Ref<T>& r) {
    return r.get() != nullptr;
  }

  template<typename T,
           typename ...Args>
  Ref<T> make_ref(Args&&...args) {
    return Ref<T>(new T(std::forward<Args>(args)...));
  }

  // AddRef() is like Ref() ctor but adding a new ref (useful to
  // create a new ref from a raw pointer).
  template<typename T>
  inline Ref<T> AddRef(T* r) {
    if (r) r->ref();
    return Ref<T>(r);
  }

} // namespace base

#endif
