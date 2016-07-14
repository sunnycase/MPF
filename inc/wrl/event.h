//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// Code in details namespace is for internal usage within the library code
//

#ifndef _WRL_EVENT_H_
#define _WRL_EVENT_H_

#ifdef _MSC_VER
#pragma once
#endif  // _MSC_VER

#include <wrl\def.h>
#include <wrl\internal.h>
#include <wrl\client.h>
#include <wrl\implements.h>
#include <wrl\wrappers\corewrappers.h>

// Set packing
#include <pshpack8.h>

#ifdef ____x_Windows_CFoundation_CIDeferral_FWD_DEFINED__
namespace ABI {
namespace Windows {
namespace Foundation {
typedef ::Windows::Foundation::IDeferral IDeferral;
typedef ::Windows::Foundation::IDeferralFactory IDeferralFactory;
typedef ::Windows::Foundation::IDeferralCompletedHandler IDeferralCompletedHandler;
}
}
}
#endif

namespace Microsoft { 
namespace WRL {

// Enum to specify the behavior on checking delegate return results
enum DelegateCheckMode
{
    NoCheck = 1
};

template<DelegateCheckMode delegateCheckMode> struct DelegateTraits;

template<>
struct DelegateTraits<NoCheck>
{
    static HRESULT CheckReturn(HRESULT hr)
    {
        return hr;
    }
};

// Enum to specify the behavior when firing event delegates
enum InvokeMode
{
   StopOnFirstError = 1,
   FireAll = 2,
};

// Event error options 
template<InvokeMode invokeModeValue>
struct InvokeModeOptions
{
    static const InvokeMode invokeMode = invokeModeValue;
};

// Forward Declaration
template<InvokeMode invokeMode> struct InvokeTraits;
template<typename TDelegateInterface, typename EventSourceOptions>
class EventSource;

namespace Details
{

template<typename TDelegateInterface>
void* GetDelegateBucketAssist(TDelegateInterface *pDelegate)
{
    // By ABI contract, delegates satisfy the below as a mechanism of getting at the invocation
    // function (the fourth slot in the V-Table of the delegate interface).  We do not care about 
    // the signature of the function, only its address as a means of improving bucketing.  
    void ***pVT = reinterpret_cast<void ***>(pDelegate);
    return (*pVT)[3];
}

// ArgTraits allows to determine amount of parameters
// on Invoke method of delegate interface
template<typename TMemberFunction>
struct ArgTraits
{
    static const int args = -1; // Indicates that we cannot match Invoke method signature
};

template<typename TDelegateInterface>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(void)>
{
    static const int args = 0;
};

template<typename TDelegateInterface, typename TArg1>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1)>
{
    static const int args = 1;
    typedef TArg1 Arg1Type;    
};

template<typename TDelegateInterface, typename TArg1, typename TArg2>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2)>
{
    static const int args = 2;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3)>
{
    static const int args = 3;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4)>
{
    static const int args = 4;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4, TArg5)>
{
    static const int args = 5;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
    typedef TArg5 Arg5Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)>
{
    static const int args = 6;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
    typedef TArg5 Arg5Type;
    typedef TArg6 Arg6Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)>
{
    static const int args = 7;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
    typedef TArg5 Arg5Type;
    typedef TArg6 Arg6Type;
    typedef TArg7 Arg7Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8)>
{
    static const int args = 8;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
    typedef TArg5 Arg5Type;
    typedef TArg6 Arg6Type;
    typedef TArg7 Arg7Type;
    typedef TArg8 Arg8Type;
};

template<typename TDelegateInterface, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9>
struct ArgTraits<HRESULT (STDMETHODCALLTYPE TDelegateInterface::*)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg9)>
{
    static const int args = 9;
    typedef TArg1 Arg1Type;
    typedef TArg2 Arg2Type;
    typedef TArg3 Arg3Type;
    typedef TArg4 Arg4Type;
    typedef TArg5 Arg5Type;
    typedef TArg6 Arg6Type;
    typedef TArg7 Arg7Type;
    typedef TArg8 Arg8Type;
    typedef TArg9 Arg9Type;
};

// Traits factory extract appropriate ArgTraits type 
// for delegate interface
template<typename TDelegateInterface, bool isImplements = __is_base_of(ImplementsBase, TDelegateInterface)>
struct ArgTraitsHelper;

// Specialization for the none Implements based interface
template<typename TDelegateInterface>
struct ArgTraitsHelper<TDelegateInterface, false>
{
    typedef decltype(&TDelegateInterface::Invoke) methodType;
    typedef ArgTraits<methodType> Traits;
    static const int args = Traits::args;
    typedef TDelegateInterface Interface;
    // Make sure that you are using STDMETHOD macro to define delegate interfaces    
    static_assert(args != -1, "Delegate Invoke signature doesn't match. Possible reasons: wrong calling convention, wrong returned type or passed too many parameters to 'Callback'");
};

// Specialization for Implements based interface
template<typename TDelegateInterface>
struct ArgTraitsHelper<TDelegateInterface, true> : ArgTraitsHelper<typename TDelegateInterface::FirstInterface>
{
};

// Invoke helper provide implementation of invoke method
// depending on amount and type of arguments from ArgTraitsHelper
template<typename TDelegateInterface, typename TCallback, unsigned int argCount>
struct InvokeHelper;

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 0> WrlSealed : public ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)()
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_());
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 1> WrlSealed : public ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{    
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(typename Traits::Arg1Type arg1)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 2> WrlSealed : public ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1,
            typename Traits::Arg2Type arg2)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 3> WrlSealed : public ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{    
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1,
            typename Traits::Arg2Type arg2,
            typename Traits::Arg3Type arg3)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 4> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1,
            typename Traits::Arg2Type arg2,
            typename Traits::Arg3Type arg3,
            typename Traits::Arg4Type arg4)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 5> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback) 
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1, 
            typename Traits::Arg2Type arg2, 
            typename Traits::Arg3Type arg3, 
            typename Traits::Arg4Type arg4,
            typename Traits::Arg5Type arg5)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4, arg5));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 6> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;   

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1, 
            typename Traits::Arg2Type arg2, 
            typename Traits::Arg3Type arg3, 
            typename Traits::Arg4Type arg4,
            typename Traits::Arg5Type arg5,
            typename Traits::Arg6Type arg6)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4, arg5, arg6));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 7> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;       

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1, 
            typename Traits::Arg2Type arg2, 
            typename Traits::Arg3Type arg3, 
            typename Traits::Arg4Type arg4,
            typename Traits::Arg5Type arg5,
            typename Traits::Arg6Type arg6,
            typename Traits::Arg7Type arg7)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 8> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1, 
            typename Traits::Arg2Type arg2, 
            typename Traits::Arg3Type arg3, 
            typename Traits::Arg4Type arg4,
            typename Traits::Arg5Type arg5,
            typename Traits::Arg6Type arg6,
            typename Traits::Arg7Type arg7,
            typename Traits::Arg8Type arg8)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
    }
    TCallback callback_;
};

template<typename TDelegateInterface, typename TCallback>
struct InvokeHelper<TDelegateInterface, TCallback, 9> WrlSealed : ::Microsoft::WRL::RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
{
    typedef typename ArgTraitsHelper<TDelegateInterface>::Traits Traits;

    explicit InvokeHelper(TCallback callback) throw() : callback_(callback)
    {
    }

    STDMETHOD(Invoke)(
            typename Traits::Arg1Type arg1, 
            typename Traits::Arg2Type arg2, 
            typename Traits::Arg3Type arg3, 
            typename Traits::Arg4Type arg4,
            typename Traits::Arg5Type arg5,
            typename Traits::Arg6Type arg6,
            typename Traits::Arg7Type arg7,
            typename Traits::Arg8Type arg8,
            typename Traits::Arg9Type arg9)
    {
        return DelegateTraits<DefaultDelegateCheckMode>::CheckReturn(callback_(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
    }
    TCallback callback_;
};


template<typename TDelegateInterface>
struct AgileInvokeHelper<TDelegateInterface, 3> : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, TDelegateInterface, Microsoft::WRL::FtmBase>
{
    typedef typename Microsoft::WRL::Details::ArgTraitsHelper<TDelegateInterface>::Traits Traits;

public:
    HRESULT Initialize(_In_ TDelegateInterface *delegateInterface)
    {
        return Microsoft::WRL::AsAgile(delegateInterface, &_agileptr);
    }

    virtual HRESULT STDMETHODCALLTYPE Invoke(
        typename Traits::Arg1Type arg1,
        typename Traits::Arg2Type arg2,
        typename Traits::Arg3Type arg3)
    {
        ComPtr<TDelegateInterface> localDelegate;

        HRESULT hr = _agileptr.CopyTo(localDelegate.GetAddressOf());
        if (SUCCEEDED(hr))
        {
            hr = localDelegate->Invoke(arg1, arg2, arg3);
        }
        return hr;
    }

private:
    AgileRef _agileptr;
};

template<typename TDelegateInterface>
HRESULT CreateAgileHelper(_In_ TDelegateInterface *delegateInterface, _COM_Outptr_ TDelegateInterface** wrapper)
{
    *wrapper = nullptr;
    ComPtr<AgileInvokeHelper<TDelegateInterface, Microsoft::WRL::Details::ArgTraitsHelper<TDelegateInterface>::args>> invokeHelper;

    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");

    invokeHelper = Make<AgileInvokeHelper<TDelegateInterface, Microsoft::WRL::Details::ArgTraitsHelper<TDelegateInterface>::args>>();
    HRESULT hr = invokeHelper ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = invokeHelper->Initialize(delegateInterface);
        if (SUCCEEDED(hr))
        {
            hr = invokeHelper.CopyTo(wrapper);
        }
    }
    return hr;
}

#endif //(NTDDI_VERSION >= NTDDI_WINBLUE)

} // namespace Details

// Implementation of Callback helper that wire delegate interfaces
// and provide object implementation for Invoke method
// Specialization for lambda, function pointer and functor
template<typename TDelegateInterface, typename TCallback>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(TCallback callback) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");

    return Make<Details::InvokeHelper<TDelegateInterface, TCallback, 
        Details::ArgTraitsHelper<TDelegateInterface>::args>>(callback);
}

// Specialization for pointer to the method
template<typename TDelegateInterface, typename TCallbackObject>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)()) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 0, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<ClassicCom>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)()) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)()
        {
            return (object_->*method_)();
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)();
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 1, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    
    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1)
        {
            return (object_->*method_)(arg1);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2)) throw()
{    
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 2, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2)
        {
            return (object_->*method_)(arg1, arg2);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 3, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>   
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3)
        {
            return (object_->*method_)(arg1, arg2, arg3);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 4, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 5, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");
    static_assert(Details::IsSame<TArg5, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg5Type>::value, "Argument 5 from object method doesn't match Invoke argument 5");
    
    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4, arg5);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4, TArg5);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 6, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");
    static_assert(Details::IsSame<TArg5, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg5Type>::value, "Argument 5 from object method doesn't match Invoke argument 5");
    static_assert(Details::IsSame<TArg6, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg6Type>::value, "Argument 6 from object method doesn't match Invoke argument 6");
    
    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4, arg5, arg6);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 7, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");
    static_assert(Details::IsSame<TArg5, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg5Type>::value, "Argument 5 from object method doesn't match Invoke argument 5");
    static_assert(Details::IsSame<TArg6, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg6Type>::value, "Argument 6 from object method doesn't match Invoke argument 6");
    static_assert(Details::IsSame<TArg7, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg7Type>::value, "Argument 7 from object method doesn't match Invoke argument 7");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 8, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");
    static_assert(Details::IsSame<TArg5, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg5Type>::value, "Argument 5 from object method doesn't match Invoke argument 5");
    static_assert(Details::IsSame<TArg6, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg6Type>::value, "Argument 6 from object method doesn't match Invoke argument 6");
    static_assert(Details::IsSame<TArg7, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg7Type>::value, "Argument 7 from object method doesn't match Invoke argument 7");
    static_assert(Details::IsSame<TArg8, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg8Type>::value, "Argument 8 from object method doesn't match Invoke argument 8");

    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8);
    };

    return Make<ComObject>(object, method);
}

template<typename TDelegateInterface, typename TCallbackObject, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9>
ComPtr<typename Details::ArgTraitsHelper<TDelegateInterface>::Interface> Callback(_In_ TCallbackObject *object, _In_ HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg9)) throw()
{
    static_assert(__is_base_of(IUnknown, TDelegateInterface) && !__is_base_of(IInspectable, TDelegateInterface), "Delegates objects must be 'IUnknown' base and not 'IInspectable'");
    static_assert(Details::ArgTraitsHelper<TDelegateInterface>::Traits::args == 9, "Number of arguments on object method doesn't match number of arguments on Delegate::Invoke");
    static_assert(Details::IsSame<TArg1, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg1Type>::value, "Argument 1 from object method doesn't match Invoke argument 1");
    static_assert(Details::IsSame<TArg2, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg2Type>::value, "Argument 2 from object method doesn't match Invoke argument 2");
    static_assert(Details::IsSame<TArg3, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg3Type>::value, "Argument 3 from object method doesn't match Invoke argument 3");
    static_assert(Details::IsSame<TArg4, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg4Type>::value, "Argument 4 from object method doesn't match Invoke argument 4");
    static_assert(Details::IsSame<TArg5, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg5Type>::value, "Argument 5 from object method doesn't match Invoke argument 5");
    static_assert(Details::IsSame<TArg6, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg6Type>::value, "Argument 6 from object method doesn't match Invoke argument 6");
    static_assert(Details::IsSame<TArg7, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg7Type>::value, "Argument 7 from object method doesn't match Invoke argument 7");
    static_assert(Details::IsSame<TArg8, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg8Type>::value, "Argument 8 from object method doesn't match Invoke argument 8");
    static_assert(Details::IsSame<TArg9, Details::ArgTraitsHelper<TDelegateInterface>::Traits::Arg9Type>::value, "Argument 9 from object method doesn't match Invoke argument 9");
    
    struct ComObject WrlSealed : RuntimeClass<RuntimeClassFlags<Delegate>, TDelegateInterface>
    {
        ComObject(TCallbackObject *object, HRESULT (TCallbackObject::* method)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg9)) throw() : object_(object), method_(method)
        {
        }

        STDMETHOD(Invoke)(TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8, TArg9 arg9)
        {
            return (object_->*method_)(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        }

        TCallbackObject* object_;
        HRESULT (TCallbackObject::* method_)(TArg1, TArg2, TArg3, TArg4, TArg5, TArg6, TArg7, TArg8, TArg9);
    };

    return Make<ComObject>(object, method);
}

namespace Details
{

// EventTargetArray is used to keep array of event targets. This array is fixed-length.
// Every time element is added/removed from array EventSource allocate new array. This array
// is optimize-for-invoke lock strategy in EventSource
class EventTargetArray WrlSealed : public ::Microsoft::WRL::RuntimeClass< ::Microsoft::WRL::RuntimeClassFlags<ClassicCom>, IUnknown >
{
    public:
        EventTargetArray() throw() : begin_(nullptr), end_(nullptr), bucketAssists_(nullptr)
        {
        }
        
        HRESULT RuntimeClassInitialize(size_t items) throw()
        {
            begin_ = new(std::nothrow) ComPtr<IUnknown>[items];
            bucketAssists_ = new(std::nothrow) void *[items];
            if (begin_ == nullptr || bucketAssists_ == nullptr)
            {
                // Don't check against nullptr because delete does it
                delete[] begin_;
                delete[] bucketAssists_;

                return E_OUTOFMEMORY;
            }

            end_ = begin_;
            return S_OK;
        }

        ~EventTargetArray() throw()
        {
            // Don't check against nullptr because delete does it
            delete[] begin_;
            delete[] bucketAssists_;
        }

        ComPtr<IUnknown>* Begin() throw()
        {
            return begin_;
        }

        ComPtr<IUnknown>* End() throw()
        {
            return end_;
        }

        void AddTail(_In_ IUnknown* element) throw()
        {
            AddTail(element, nullptr);
        }         

        void AddTail(_In_ IUnknown* element, void *bucketAssist) throw()
        {
            // We'll run over the end if you call AddTail too many times, but the alternate would be an extra variable
            // to keep track of the number of items allocated. This class is only privately used by EventSourceBase.
            *end_ = element;
            *(bucketAssists_ + (end_ - begin_)) = bucketAssist;
            end_++;
        }

        size_t Length() throw()
        {
            return (end_ - begin_);
        }

        void **Begin_BucketAssists()
        {
            return bucketAssists_;
        }

        void **End_BucketAssists()
        {
            return bucketAssists_ + (end_ - begin_);
        }

    private:
        ComPtr<IUnknown>*  begin_;
        ComPtr<IUnknown>*  end_;
        void **bucketAssists_;
};

} // namespace Details

template<>
struct InvokeTraits<FireAll>
{
   template<typename TInvokeMethod, typename TDelegateInterface>
   static HRESULT InvokeDelegates(TInvokeMethod invokeOne, Details::EventTargetArray *targetArray, EventSource<TDelegateInterface, InvokeModeOptions<FireAll>>* pEvent)
   {
      ComPtr<Details::EventTargetArray> targets;
      targets = targetArray;

      for (auto element = targets->Begin(); element != targets->End(); element++)
      {
          HRESULT hr = (invokeOne)(*element);
          if (FAILED(hr)) 
          {
              ::RoTransformError(hr, S_OK, nullptr);                    
              // Remove event that is already disconnected
              if (hr == RPC_E_DISCONNECTED || hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) || hr == JSCRIPT_E_CANTEXECUTE)
              {
                  EventRegistrationToken token;
                  token.value = reinterpret_cast<__int64>(element->Get());
                  pEvent->Remove(token);
              }
          }
      }
      return S_OK;
   }
};

template<>
struct InvokeTraits<StopOnFirstError>
{
   template<typename TInvokeMethod, typename TDelegateInterface>
   static HRESULT InvokeDelegates(TInvokeMethod invokeOne, Details::EventTargetArray *targetArray, EventSource<TDelegateInterface, InvokeModeOptions<StopOnFirstError>>* pEvent)
   {
      HRESULT hr = S_OK;
      ComPtr<Details::EventTargetArray> targets;
      targets = targetArray;

      for (auto element = targets->Begin(); element != targets->End(); element++)
      {
           hr = (invokeOne)(*element);
           // Remove event that is already disconnected
           if (hr == RPC_E_DISCONNECTED || hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE) || hr == JSCRIPT_E_CANTEXECUTE)
           {
               // if we get the above errors, treat it as success and unregister the delegate
               ::RoTransformError(hr, S_OK, nullptr);   
               EventRegistrationToken token;
               token.value = reinterpret_cast<__int64>(element->Get());
               pEvent->Remove(token);
               hr = S_OK;
           }
           if (FAILED(hr))
           {
              // break out of the loop on the first unhandled error
              break;
           }
      }
      return hr;
   }
};

#ifdef __windows2Efoundation_h__

template <typename TEventArgsInterface, typename TEventArgsClass>
class DeferrableEventArgs : public TEventArgsInterface
{
public:
    STDMETHOD(GetDeferral)(_COM_Outptr_ ::ABI::Windows::Foundation::IDeferral** result)
    {
        *result = nullptr;
        auto lockGuard = lock_.LockExclusive();
        if (raised_)
        {
            // Cannot ask for a deferral after the event handler returned.
            ::RoOriginateError(E_ILLEGAL_METHOD_CALL, nullptr);
            return E_ILLEGAL_METHOD_CALL;
        }

        Microsoft::WRL::ComPtr< ::ABI::Windows::Foundation::IDeferralFactory > factory;
        HRESULT hr = GetActivationFactory(
            Wrappers::HStringReference(RuntimeClass_Windows_Foundation_Deferral).Get(),
            &factory);
        if (FAILED(hr))
        {
            return hr;
        }

        Microsoft::WRL::ComPtr<DeferrableEventArgs> lifetime(this);
        auto callback = Microsoft::WRL::Callback< ::ABI::Windows::Foundation::IDeferralCompletedHandler >(
                [lifetime]() { return lifetime->Complete(); });
        if (callback == nullptr)
        {
            return E_OUTOFMEMORY;
        }

        Microsoft::WRL::ComPtr< ::ABI::Windows::Foundation::IDeferral > deferral;
        hr = factory->Create(callback.Get(), &deferral);
        if (FAILED(hr))
        {
            return hr;
        }

        completionsRequired_++;
        return deferral.CopyTo(result);
    }

    // InvokeAllFinished() should be called after the event source calls InvokeAll.  This will prevent further
    // deferrals from being taken, and cause the completion handler to execute if no deferrals were taken.
    void InvokeAllFinished()
    {
        bool invokeNeeded;

        // We need to hold a lock while modifying private state, but release it before invoking a completion handler.
        {
            auto lockGuard = lock_.LockExclusive();
            raised_ = true;
            invokeNeeded = (completionsRequired_ == 0);
        }

        if (invokeNeeded)
        {
            static_cast<TEventArgsClass*>(this)->InvokeCompleteHandler();
        }
    }

private:
    _Requires_lock_not_held_(lock_)
    HRESULT Complete()
    {
        bool invokeNeeded;

        // We need to hold a lock while modifying private state, but release it before invoking a completion handler.
        {
            auto lockGuard = lock_.LockExclusive();
            if (completionsRequired_ == 0)
            {
                // This should never happen since Complete() should only be called by Windows.Foundation.Deferral
                // which will only invoke our completion handler once.
                ::RoOriginateError(E_ILLEGAL_METHOD_CALL, nullptr);
                return E_ILLEGAL_METHOD_CALL;
            }
            completionsRequired_--;
            invokeNeeded = (raised_ && (completionsRequired_ == 0));
        }

        if (invokeNeeded)
        {
            static_cast<TEventArgsClass*>(this)->InvokeCompleteHandler();
        }

        return S_OK;
    }

    Wrappers::SRWLock lock_;
    _Guarded_by_(lock_)
    bool raised_ = false;
    _Guarded_by_(lock_)
    long completionsRequired_ = 0;
};

#endif // __windows2Efoundation_h__


} } // namespace ::Microsoft::WRL

// Restore packing
#include <poppack.h>
    
#endif // _WRL_EVENT_H_

#ifdef BUILD_WINDOWS
#include <wrl\internalevent.h>
#else
namespace Microsoft
{
namespace WRL
{
extern __declspec(selectany) const DelegateCheckMode DefaultDelegateCheckMode = NoCheck;
} } // namespace ::Microsoft::WRL
#endif
