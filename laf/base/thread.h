// LAF Base Library
// Copyright (C) 2019-2021  Igara Studio S.A.
// Copyright (C) 2001-2016  David Capello
//
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#ifndef BASE_THREAD_H_INCLUDED
#define BASE_THREAD_H_INCLUDED
#pragma once

#if LAF_WINDOWS
  #include "base/ints.h"
#endif

namespace base {                // Based on C++0x threads lib

  class thread {
  public:
    typedef void* native_handle_type;
#if LAF_WINDOWS
    typedef uint32_t native_id_type;
#else
    typedef void* native_id_type;
#endif

    // Create an instance to represent the current thread
    thread();

    // Create a new thread without arguments
    template<class Callable>
    thread(const Callable& f) {
      launch_thread(new func_wrapper0<Callable>(f));
    }

    // Create a new thread with one argument
    template<class Callable, class A>
    thread(const Callable& f, A a) {
      launch_thread(new func_wrapper1<Callable, A>(f, a));
    }

    // Create a new thread with two arguments
    template<class Callable, class A, class B>
    thread(const Callable& f, A a, B b) {
      launch_thread(new func_wrapper2<Callable, A, B>(f, a, b));
    }

    ~thread();

    bool joinable() const;
    void join();
    void detach();

    native_id_type native_id() const;

    class details {
    public:
      static void thread_proxy(void* data);
    };

  private:
    native_handle_type m_native_handle;
#if LAF_WINDOWS
    native_id_type m_native_id;
#endif

    class func_wrapper {
    public:
      virtual ~func_wrapper() { }
      virtual void operator()() = 0;
    };

    void launch_thread(func_wrapper* f);

    template<class Callable>
    class func_wrapper0 : public func_wrapper {
    public:
      Callable f;
      func_wrapper0(const Callable& f) : f(f) { }
      void operator()() { f(); }
    };

    template<class Callable, class A>
    class func_wrapper1 : public func_wrapper {
    public:
      Callable f;
      A a;
      func_wrapper1(const Callable& f, A a) : f(f), a(a) { }
      void operator()() { f(a); }
    };

    template<class Callable, class A, class B>
    class func_wrapper2 : public func_wrapper {
    public:
      Callable f;
      A a;
      B b;
      func_wrapper2(const Callable& f, A a, B b) : f(f), a(a), b(b) { }
      void operator()() { f(a, b); }
    };

  };

  namespace this_thread
  {
    void yield();
    void sleep_for(double seconds);
    thread::native_id_type native_id();
  }

  // This class joins the thread in its destructor.
  class thread_guard {
    thread& m_thread;
  public:
    explicit thread_guard(thread& t) : m_thread(t) { }
    ~thread_guard()
    {
      if (m_thread.joinable())
        m_thread.join();
    }
  };

}

#endif
