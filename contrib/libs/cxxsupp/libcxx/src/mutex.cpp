//===------------------------- mutex.cpp ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mutex"
#include "limits"
#include "system_error"
#if !defined(_LIBCPP_ABI_MICROSOFT)
#include "include/atomic_support.h"
#endif
#include "__undef_macros"

#ifndef _LIBCPP_HAS_NO_THREADS
#if defined(__ELF__) && defined(_LIBCPP_LINK_PTHREAD_LIB)
#pragma comment(lib, "pthread")
#endif
#endif

_LIBCPP_BEGIN_NAMESPACE_STD
#ifndef _LIBCPP_HAS_NO_THREADS

const defer_lock_t  defer_lock{};
const try_to_lock_t try_to_lock{};
const adopt_lock_t  adopt_lock{};

// ~mutex is defined elsewhere

void
mutex::lock()
{
    int ec = __libcpp_mutex_lock(&__m_);
    if (ec)
        __throw_system_error(ec, "mutex lock failed");
}

bool
mutex::try_lock() noexcept
{
    return __libcpp_mutex_trylock(&__m_);
}

void
mutex::unlock() noexcept
{
    int ec = __libcpp_mutex_unlock(&__m_);
    (void)ec;
    _LIBCPP_ASSERT(ec == 0, "call to mutex::unlock failed");
}

// recursive_mutex

recursive_mutex::recursive_mutex()
{
    int ec = __libcpp_recursive_mutex_init(&__m_);
    if (ec)
        __throw_system_error(ec, "recursive_mutex constructor failed");
}

recursive_mutex::~recursive_mutex()
{
    int e = __libcpp_recursive_mutex_destroy(&__m_);
    (void)e;
    _LIBCPP_ASSERT(e == 0, "call to ~recursive_mutex() failed");
}

void
recursive_mutex::lock()
{
    int ec = __libcpp_recursive_mutex_lock(&__m_);
    if (ec)
        __throw_system_error(ec, "recursive_mutex lock failed");
}

void
recursive_mutex::unlock() noexcept
{
    int e = __libcpp_recursive_mutex_unlock(&__m_);
    (void)e;
    _LIBCPP_ASSERT(e == 0, "call to recursive_mutex::unlock() failed");
}

bool
recursive_mutex::try_lock() noexcept
{
    return __libcpp_recursive_mutex_trylock(&__m_);
}

// timed_mutex

timed_mutex::timed_mutex()
    : __locked_(false)
{
}

timed_mutex::~timed_mutex()
{
    lock_guard<mutex> _(__m_);
}

void
timed_mutex::lock()
{
    unique_lock<mutex> lk(__m_);
    while (__locked_)
        __cv_.wait(lk);
    __locked_ = true;
}

bool
timed_mutex::try_lock() noexcept
{
    unique_lock<mutex> lk(__m_, try_to_lock);
    if (lk.owns_lock() && !__locked_)
    {
        __locked_ = true;
        return true;
    }
    return false;
}

void
timed_mutex::unlock() noexcept
{
    lock_guard<mutex> _(__m_);
    __locked_ = false;
    __cv_.notify_one();
}

// recursive_timed_mutex

recursive_timed_mutex::recursive_timed_mutex()
    : __count_(0),
      __id_{}
{
}

recursive_timed_mutex::~recursive_timed_mutex()
{
    lock_guard<mutex> _(__m_);
}

void
recursive_timed_mutex::lock()
{
    __thread_id id = this_thread::get_id();
    unique_lock<mutex> lk(__m_);
    if (id ==__id_)
    {
        if (__count_ == numeric_limits<size_t>::max())
            __throw_system_error(EAGAIN, "recursive_timed_mutex lock limit reached");
        ++__count_;
        return;
    }
    while (__count_ != 0)
        __cv_.wait(lk);
    __count_ = 1;
    __id_ = id;
}

bool
recursive_timed_mutex::try_lock() noexcept
{
    __thread_id id = this_thread::get_id();
    unique_lock<mutex> lk(__m_, try_to_lock);
    if (lk.owns_lock() && (__count_ == 0 || id == __id_))
    {
        if (__count_ == numeric_limits<size_t>::max())
            return false;
        ++__count_;
        __id_ = id;
        return true;
    }
    return false;
}

void
recursive_timed_mutex::unlock() noexcept
{
    unique_lock<mutex> lk(__m_);
    if (--__count_ == 0)
    {
        __id_.__reset();
        lk.unlock();
        __cv_.notify_one();
    }
}

#endif // !_LIBCPP_HAS_NO_THREADS

// If dispatch_once_f ever handles C++ exceptions, and if one can get to it
// without illegal macros (unexpected macros not beginning with _UpperCase or
// __lowercase), and if it stops spinning waiting threads, then call_once should
// call into dispatch_once_f instead of here. Relevant radar this code needs to
// keep in sync with:  7741191.

#ifndef _LIBCPP_HAS_NO_THREADS
_LIBCPP_SAFE_STATIC static __libcpp_mutex_t mut = _LIBCPP_MUTEX_INITIALIZER;
_LIBCPP_SAFE_STATIC static __libcpp_condvar_t cv = _LIBCPP_CONDVAR_INITIALIZER;
#endif

#ifdef _LIBCPP_ABI_MICROSOFT
void __call_once(volatile std::atomic<once_flag::_State_type>& flag, void* arg,
                 void (*func)(void*))
#else
void __call_once(volatile once_flag::_State_type& flag, void* arg,
                 void (*func)(void*))
#endif
{
#if defined(_LIBCPP_HAS_NO_THREADS)
    if (flag == 0)
    {
#ifndef _LIBCPP_NO_EXCEPTIONS
        try
        {
#endif // _LIBCPP_NO_EXCEPTIONS
            flag = 1;
            func(arg);
            flag = ~once_flag::_State_type(0);
#ifndef _LIBCPP_NO_EXCEPTIONS
        }
        catch (...)
        {
            flag = 0;
            throw;
        }
#endif // _LIBCPP_NO_EXCEPTIONS
    }
#else // !_LIBCPP_HAS_NO_THREADS
    __libcpp_mutex_lock(&mut);
    while (flag == 1)
        __libcpp_condvar_wait(&cv, &mut);
    if (flag == 0)
    {
#ifndef _LIBCPP_NO_EXCEPTIONS
        try
        {
#endif // _LIBCPP_NO_EXCEPTIONS
#ifdef _LIBCPP_ABI_MICROSOFT
            flag.store(once_flag::_State_type(1));
#else
            __libcpp_relaxed_store(&flag, once_flag::_State_type(1));
#endif
            __libcpp_mutex_unlock(&mut);
            func(arg);
            __libcpp_mutex_lock(&mut);
#ifdef _LIBCPP_ABI_MICROSOFT
            flag.store(~once_flag::_State_type(0), memory_order_release);
#else
            __libcpp_atomic_store(&flag, ~once_flag::_State_type(0),
                                  _AO_Release);
#endif
            __libcpp_mutex_unlock(&mut);
            __libcpp_condvar_broadcast(&cv);
#ifndef _LIBCPP_NO_EXCEPTIONS
        }
        catch (...)
        {
            __libcpp_mutex_lock(&mut);
#ifdef _LIBCPP_ABI_MICROSOFT
            flag.store(once_flag::_State_type(0), memory_order_relaxed);
#else
            __libcpp_relaxed_store(&flag, once_flag::_State_type(0));
#endif
            __libcpp_mutex_unlock(&mut);
            __libcpp_condvar_broadcast(&cv);
            throw;
        }
#endif // _LIBCPP_NO_EXCEPTIONS
    }
    else
        __libcpp_mutex_unlock(&mut);
#endif // !_LIBCPP_HAS_NO_THREADS
}

_LIBCPP_END_NAMESPACE_STD
