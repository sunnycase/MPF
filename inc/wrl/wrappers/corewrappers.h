//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// Code in details namespace is for internal usage within the library code
//

#ifndef _WRL_COREWRAPPERS_H_
#define _WRL_COREWRAPPERS_H_

#ifdef _MSC_VER
#pragma once
#endif  // _MSC_VER

#include <windows.h>
#include <intsafe.h>
#ifdef BUILD_WINDOWS
#include <winrt.h>
#endif

#include <wrl\def.h>
#include <wrl\internal.h>

// Set packing
#include <pshpack8.h>

namespace Microsoft {
namespace WRL {

namespace Details {
    // Need a type that isn't assignable from int (see other overloads of Initialize, that accept int as second arg)
    // this is needed as part of EnableIf
    struct Dummy {};
}

namespace Wrappers {

namespace HandleTraits
{
// Handle specializations for implemented RAII wrappers
struct HANDLENullTraits
{
    typedef HANDLE Type;

    inline static bool Close(_In_ Type h) throw()
    {
        return ::CloseHandle(h) != FALSE;
    }

    inline static Type GetInvalidValue() throw()
    { 
        return nullptr; 
    }
};

struct HANDLETraits
{
    typedef HANDLE Type;

    inline static bool Close(_In_ Type h) throw()
    {
        return ::CloseHandle(h) != FALSE;
    }

    inline static HANDLE GetInvalidValue() throw()
    { 
        return INVALID_HANDLE_VALUE; 
    }
};

struct FileHandleTraits : HANDLETraits
{
};

struct CriticalSectionTraits
{
    typedef CRITICAL_SECTION* Type;

    inline static Type GetInvalidValue() throw() 
    { 
        return nullptr; 
    }

    _Releases_lock_(*cs)
    inline static void Unlock(_In_ Type cs) throw()
    {
        ::LeaveCriticalSection(cs);
    }
};

struct MutexTraits : HANDLENullTraits
{
    _Releases_lock_(h)
    inline static void Unlock(_In_ Type h) throw()
    {
        if (::ReleaseMutex(h) == FALSE)
        {
            // If we cannot release mutex it indicates 
            // bug in somebody code thus we raise an exception
            ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
};

struct SemaphoreTraits : HANDLENullTraits
{
    inline static void Unlock(_In_ Type h) throw()
    {
        if (::ReleaseSemaphore(h, 1, NULL) == FALSE)
        {
            // If we cannot release semaphore it indicates 
            // bug in somebody code thus we raise an exception
            ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
};

struct EventTraits : HANDLENullTraits
{
};

struct SRWLockSharedTraits
{
    typedef SRWLOCK* Type;

    inline static Type GetInvalidValue() throw()
    {
        return nullptr; 
    }

    _Releases_shared_lock_(*srwlock)
    inline static void Unlock(_In_ Type srwlock) throw()
    {
        ::ReleaseSRWLockShared(srwlock);
    }
};

struct SRWLockExclusiveTraits
{
    typedef SRWLOCK* Type;

    inline static Type GetInvalidValue() throw()
    { 
        return nullptr; 
    }

    _Releases_exclusive_lock_(*srwlock)
    inline static void Unlock(_In_ Type srwlock) throw()
    {
        ::ReleaseSRWLockExclusive(srwlock);
    }
};

} // namespace HandleTraits

// Handle object implementation specialized with traits
template <typename HandleTraits>
class HandleT
{
public:
    explicit HandleT(typename HandleTraits::Type h = HandleTraits::GetInvalidValue()) throw() : 
        handle_(h)
    {
    }

    HandleT(_Inout_ HandleT&& h) throw() : handle_(h.handle_)
    {
        h.handle_ = HandleTraits::GetInvalidValue();
    }

    ~HandleT() throw()
    {
        Close();
    }

    HandleT& operator=(_Inout_ HandleT&& h) throw()
    {
        Close();
        handle_ = h.handle_;
        h.handle_ = HandleTraits::GetInvalidValue();
        return *this;
    }

    void Attach(typename HandleTraits::Type h) throw()
    {
        if (h != handle_)
        {
            Close();
            handle_ = h;
        }
    }

    typename HandleTraits::Type Detach() throw()
    {
        typename HandleTraits::Type h = handle_;
        handle_ = HandleTraits::GetInvalidValue();
        return h;
    }

    typename HandleTraits::Type Get() const throw()
    { 
        return handle_;
    }

    void Close() throw()
    {
        if (handle_ != HandleTraits::GetInvalidValue())
        {
            bool ret = InternalClose();
            if (!ret)
            {
                // If we cannot close the handle it indicates
                // bug in somebody code thus we raise an exception
                ::Microsoft::WRL::Details::RaiseException(HRESULT_FROM_WIN32(GetLastError()));            
            }
            handle_ = HandleTraits::GetInvalidValue();
        }
    }

    bool IsValid() const throw()
    {
        return handle_ != HandleTraits::GetInvalidValue();
    }

    typename HandleTraits::Type* GetAddressOf() throw()
    {
        return &handle_;
    }

    typename HandleTraits::Type* ReleaseAndGetAddressOf() throw()
    {
        Close();
        return &handle_;
    }

    typedef HandleTraits Traits;
protected:
    virtual bool InternalClose() throw()
    {
        return HandleTraits::Close(handle_);
    }
    
    typename HandleTraits::Type handle_;
private:
    HandleT(const HandleT&);
    HandleT& operator=(const HandleT&);
};

// HandleT comparison operators
template<class T> 
bool operator==(const HandleT<T>& rhs, const HandleT<T>& lhs) throw()
{
    return rhs.Get() == lhs.Get();
}

template<class T> 
bool operator==(const typename HandleT<T>::Traits::Type& lhs, const HandleT<T>& rhs) throw()
{
    return lhs == rhs.Get();
}

template<class T> 
bool operator==(const HandleT<T>& lhs, const typename HandleT<T>::Traits::Type& rhs) throw()
{
    return lhs.Get() == rhs;
}

template<class T>
bool operator!=(const HandleT<T>& lhs, const HandleT<T>& rhs) throw()
{
    return lhs.Get() != rhs.Get();
}

template<class T> 
bool operator!=(const typename HandleT<T>::Traits::Type& lhs, const HandleT<T>& rhs) throw()
{
    return lhs != rhs.Get();
}

template<class T> 
bool operator!=(const HandleT<T>& lhs, const typename HandleT<T>::Traits::Type& rhs) throw()
{
    return lhs.Get() != rhs;
}

template<class T>
bool operator<(const HandleT<T>& lhs, const HandleT<T>& rhs) throw()
{
    return lhs.Get() < rhs.Get();
}

typedef HandleT<HandleTraits::FileHandleTraits>          FileHandle;


// Forward declarations
class CriticalSection;
class Mutex;
class Semaphore;
class SRWLock;

namespace Details {

#pragma warning(push)
// Disable unheld lock warning for RIAA objects
#pragma warning(disable: 26165)

// Shared lock object for CriticalSection
class SyncLockCriticalSection
{
public:
    SyncLockCriticalSection(_Inout_ SyncLockCriticalSection&& other) throw() : sync_(other.sync_)
    {
        other.sync_ = HandleTraits::CriticalSectionTraits::GetInvalidValue();
    }

     _Releases_lock_(*sync_)
    ~SyncLockCriticalSection() throw()
    {
        InternalUnlock();
    }

    _Releases_lock_(*sync_)
    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return (sync_ != HandleTraits::CriticalSectionTraits::GetInvalidValue());
    }

    friend class Wrappers::CriticalSection;

protected:
    explicit SyncLockCriticalSection(CRITICAL_SECTION* sync = HandleTraits::CriticalSectionTraits::GetInvalidValue()) throw() : sync_(sync)
    {
    }

    CRITICAL_SECTION* sync_;
    
private:
    // Disallow copy and assignment
    SyncLockCriticalSection(const SyncLockCriticalSection&);
    SyncLockCriticalSection& operator=(const SyncLockCriticalSection&);

    _Releases_lock_(*sync_)
    void InternalUnlock() throw()
    {
        if (IsLocked())
        {
            // Instances of this class should be used on the stack
            // and should not be passed across threads.
            // Unlock can fail if it is called from the wrong thread
            // or with an Invalid Handle, both of which are bugs
            // Traits::Unlock should raise an SEH in case it cannot 
            // release the lock

            HandleTraits::CriticalSectionTraits::Unlock(sync_);
            sync_ = HandleTraits::CriticalSectionTraits::GetInvalidValue();
        }
    }
};

// Exclusive lock object for SRWLock
class SyncLockExclusive
{
public:
    SyncLockExclusive(_Inout_ SyncLockExclusive&& other) throw() : sync_(other.sync_)
    {
        other.sync_ = HandleTraits::SRWLockExclusiveTraits::GetInvalidValue();
    }

    _Releases_exclusive_lock_(*sync_)
    ~SyncLockExclusive() throw()
    {
        InternalUnlock();
    }

    _Releases_exclusive_lock_(*sync_)
    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return (sync_ != HandleTraits::SRWLockExclusiveTraits::GetInvalidValue());
    }

    friend class Wrappers::SRWLock;
    
protected:
    explicit SyncLockExclusive(SRWLOCK* sync = HandleTraits::SRWLockExclusiveTraits::GetInvalidValue()) throw() : sync_(sync)
    {

    }
    
    SRWLOCK* sync_;
    
private:
    // Disallow copy and assignment
    SyncLockExclusive(const SyncLockExclusive&);
    SyncLockExclusive& operator=(const SyncLockExclusive&);

    _Releases_exclusive_lock_(*sync_)
    void InternalUnlock() throw()
    {
        if(IsLocked())
        {
            HandleTraits::SRWLockExclusiveTraits::Unlock(sync_);
            sync_ = HandleTraits::SRWLockExclusiveTraits::GetInvalidValue();;
        }
    }
};

// Shared lock object for SRWLock
class SyncLockShared
{
public:
    SyncLockShared(_Inout_ SyncLockShared&& other) throw() : sync_(other.sync_)
    {
        other.sync_ = HandleTraits::SRWLockSharedTraits::GetInvalidValue();
    }

    _Releases_shared_lock_(*sync_)
    ~SyncLockShared() throw()
    {
        InternalUnlock();
    }

    _Releases_shared_lock_(*sync_)
    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return (sync_ != HandleTraits::SRWLockSharedTraits::GetInvalidValue());
    }

    friend class Wrappers::SRWLock;
    
protected:
    explicit SyncLockShared(SRWLOCK* sync = HandleTraits::SRWLockSharedTraits::GetInvalidValue()) throw() : sync_(sync)
    {

    }

    SRWLOCK* sync_;
    
private:
    // Disallow copy and assignment
    SyncLockShared(const SyncLockShared&);
    SyncLockShared& operator=(const SyncLockShared&);

    _Releases_shared_lock_(*sync_)
    void InternalUnlock() throw()
    {
        if(IsLocked())
        {
            HandleTraits::SRWLockSharedTraits::Unlock(sync_);
            sync_ = HandleTraits::SRWLockSharedTraits::GetInvalidValue();
        }
    }
};

#pragma warning(pop)

#pragma warning(push)
// Missing annotation _Releases_*
// Possibly releasing unheld lock
#pragma warning(disable: 26165 26167 26135)
// Lock object implemenatation customzed with traits
template <typename SyncTraits>
class SyncLockWithStatusT
{
public:
    SyncLockWithStatusT(_Inout_ SyncLockWithStatusT&& other) throw() : sync_(other.sync_), status_(other.status_)
    {
        other.sync_ = SyncTraits::GetInvalidValue();
    }

    ~SyncLockWithStatusT() throw()
    {
        InternalUnlock();
    }

    void Unlock() throw()
    {
        __WRL_ASSERT__(IsLocked());
        InternalUnlock();
    }

    bool IsLocked() const throw()
    {
        return sync_ != SyncTraits::GetInvalidValue() && (status_ == 0 || status_ == WAIT_ABANDONED);
    }
    // status value 0 indicates success
    DWORD GetStatus() const throw()
    {
        return status_;
    }

    friend class Wrappers::Mutex;
    friend class Wrappers::Semaphore;
protected:
    explicit SyncLockWithStatusT(typename SyncTraits::Type sync, DWORD status) throw() : 
        sync_(sync), status_(status)
    {
    }

    DWORD status_;
    typename SyncTraits::Type sync_;
private:
    // Disallow copy and assignment
    SyncLockWithStatusT(const SyncLockWithStatusT&);
    SyncLockWithStatusT& operator=(const SyncLockWithStatusT&);

    void InternalUnlock() throw()
    {
        if (IsLocked())
        {
            // Instances of this class should be used on the stack
            // and should not be passed across threads.
            // Unlock can fail if it is called from the wrong thread
            // or with an Invalid Handle, both of which are bugs
            // Traits::Unlock should raise an SEH in case it cannot 
            // release the lock
 
// Cannot use _Analysis_assume_lock_held_(sync) 
// because template instantiations have differing
// levels of indirection to the lock
#pragma warning(suppress:26110)
            SyncTraits::Unlock(sync_);
            sync_ = SyncTraits::GetInvalidValue();
        }
    }
};

#pragma warning(pop)

} // namespace Details

// Critical section implementation
class CriticalSection
{
public:
    typedef Details::SyncLockCriticalSection SyncLock;

    explicit CriticalSection(ULONG spincount = 0) throw()
    {
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
        ::InitializeCriticalSectionEx(&cs_, spincount, 0);
#else
		::InitializeCriticalSectionAndSpinCount(&cs_, spincount);
#endif
    }

    ~CriticalSection() throw()
    {
        ::DeleteCriticalSection(&cs_);
    }

    _Acquires_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, cs_)
    SyncLock Lock() throw()
    {
        return Lock(&cs_);
    }

    _Acquires_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *cs)
    static SyncLock Lock(_In_ CRITICAL_SECTION* cs) throw()
    {
        ::EnterCriticalSection(cs);
        return SyncLock(cs);
    }

    _Acquires_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, cs_)
    SyncLock TryLock() throw()
    {
        return TryLock(&cs_);
    }

    _Acquires_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *cs)
    static SyncLock TryLock(_In_ CRITICAL_SECTION* cs) throw()
    {
        bool acquired = !!::TryEnterCriticalSection(cs);
        _Analysis_assume_lock_held_(*cs);
        return SyncLock((acquired) ? cs : nullptr);
    }

    bool IsValid() const throw()
    {
        return true;
    }
protected:
    CRITICAL_SECTION cs_;
private:
    // Disallow copy and assignment
    CriticalSection(const CriticalSection&);
    CriticalSection& operator=(const CriticalSection&);
};

// Mutex handle implementation
class Mutex : public HandleT<HandleTraits::MutexTraits>
{
public:
    typedef Details::SyncLockWithStatusT<HandleTraits::MutexTraits> SyncLock;

    explicit Mutex(HANDLE h) throw() : HandleT(h)
    {
    }

    Mutex(_Inout_ Mutex&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Mutex& operator=(_Inout_ Mutex&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }

    SyncLock Lock(DWORD milliseconds = INFINITE) throw()
    {
        return Lock(Get(), milliseconds);
    }

    static SyncLock Lock(HANDLE h, DWORD milliseconds = INFINITE) throw()
    {
        DWORD const status = ::WaitForSingleObjectEx(h, milliseconds, FALSE);
        return SyncLock(h, status == WAIT_OBJECT_0 ? 0 : status);
    }    
private:
    void Close();
    HANDLE Detach();
    void Attach(HANDLE);
    HANDLE* GetAddressOf();
    HANDLE* ReleaseAndGetAddressOf();
};

// Semaphore handle implementation
class Semaphore : public HandleT<HandleTraits::SemaphoreTraits>
{
public:
    typedef Details::SyncLockWithStatusT<HandleTraits::SemaphoreTraits> SyncLock;

    explicit Semaphore(HANDLE h) throw() : HandleT(h)
    {
    }    

    Semaphore(_Inout_ Semaphore&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Semaphore& operator=(_Inout_ Semaphore&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }

    SyncLock Lock(DWORD milliseconds = INFINITE) throw()
    {
        return Lock(Get(), milliseconds);
    }

    static SyncLock Lock(HANDLE h, DWORD milliseconds = INFINITE) throw()
    {
        DWORD const status = ::WaitForSingleObjectEx(h, milliseconds, FALSE);
        return SyncLock(h, status == WAIT_OBJECT_0 ? 0 : status);
    }
private:
    void Close();
    HANDLE Detach();
    void Attach(HANDLE);
    HANDLE* GetAddressOf();
    HANDLE* ReleaseAndGetAddressOf();
};

// Event handle implementation
class Event : public HandleT<HandleTraits::EventTraits>
{
public:
    explicit Event(HANDLE h = HandleT::Traits::GetInvalidValue()) throw() : HandleT(h)
    {
    }

    Event(_Inout_ Event&& h) throw() : HandleT(::Microsoft::WRL::Details::Move(h))
    {
    }

    Event& operator=(_Inout_ Event&& h) throw()
    {
        *static_cast<HandleT*>(this) = ::Microsoft::WRL::Details::Move(h);
        return *this;
    }
};

// SRW lock implementation
class SRWLock
{
public:
    typedef Details::SyncLockExclusive SyncLockExclusive;
    typedef Details::SyncLockShared SyncLockShared;

    SRWLock() throw()
    {
        ::InitializeSRWLock(&SRWLock_);
    }

    ~SRWLock() throw()
    {
    }

    _Acquires_exclusive_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, SRWLock_)
    SyncLockExclusive LockExclusive() throw()
    {
        return LockExclusive(&SRWLock_);
    }

    _Acquires_exclusive_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *lock)
    static SyncLockExclusive LockExclusive(_In_ SRWLOCK* lock) throw()
    {
        ::AcquireSRWLockExclusive(lock);
        return SyncLockExclusive(lock);
    }

    _Acquires_exclusive_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, SRWLock_)
    SyncLockExclusive TryLockExclusive() throw()
    {
        return TryLockExclusive(&SRWLock_);
    }

    _Acquires_exclusive_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *lock)
    static SyncLockExclusive TryLockExclusive(_In_ SRWLOCK* lock) throw()
    {
        bool acquired = !!::TryAcquireSRWLockExclusive(lock);
        _Analysis_assume_lock_held_(*lock);
        return SyncLockExclusive((acquired) ? lock : nullptr);
    }

    _Acquires_shared_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, SRWLock_)
    SyncLockShared LockShared() throw()
    {
        return LockShared(&SRWLock_);
    }

    _Acquires_shared_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *lock)
    static SyncLockShared LockShared(_In_ SRWLOCK* lock) throw()
    {
        ::AcquireSRWLockShared(lock);
        return SyncLockShared(lock);
    }

    _Acquires_shared_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, SRWLock_)
    SyncLockShared TryLockShared() throw()
    {
        return TryLockShared(&SRWLock_);
    }

    _Acquires_shared_lock_(*return.sync_)
    _Post_same_lock_(*return.sync_, *lock)
    static SyncLockShared TryLockShared(_In_ SRWLOCK* lock) throw()
    {
        bool acquired = !!::TryAcquireSRWLockShared(lock);
        _Analysis_assume_lock_held_(*lock);
        return SyncLockShared((acquired) ? lock : nullptr);
    }
    
protected:
    SRWLOCK SRWLock_;
private:
    // Disallow copy and assignment
    SRWLock(const SRWLock&);
    SRWLock& operator=(const SRWLock&);
};

namespace Details {

} // namespace Details


} } } // namespace ::Microsoft::WRL::Wrappers

// Restore packing
#include <poppack.h>

#endif // _WRL_COREWRAPPERS_H_
