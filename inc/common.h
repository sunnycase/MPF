//
// MPF Platform
// 公共头文件
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#pragma once

#define DEFINE_NS_PLATFORM namespace MPF { namespace Platform {
#define END_NS_PLATFORM }}
#define NS_PLATFORM MPF::Platform

#ifndef DEFINE_NS_ONLY

#ifdef MPF_PLATFORM_EXPORTS
#define MPF_PLATFORM_API __declspec(dllexport)
#else
#define MPF_PLATFORM_API __declspec(dllimport)
#endif

#ifdef __cplusplus_winrt
#include "winrt/platform.h"
#else
#include "desktop/platform.h"
#endif

#define DEFINE_PROPERTY_GET(name, type) __declspec(property(get = get_##name)) type name
#define ARGUMENT_NOTNULL_HR(pointer) if(!(pointer)) return E_POINTER

#include <functional>
#include <memory>

///<summary>终结器</summary>
template<typename TCall>
class finalizer final
{
public:
	finalizer(TCall&& action)
		:action(std::forward<decltype(action)>(action))
	{

	}

	finalizer(finalizer&&) = default;
	finalizer& operator=(finalizer&&) = default;

	~finalizer()
	{
		action();
	}
private:
	TCall action;
};

template<typename TCall>
finalizer<TCall> make_finalizer(TCall&& action)
{
	return finalizer<TCall>(std::forward<decltype(action)>(action));
}

#ifdef _WIN32

template<class T>
struct cotaskmem_deleter
{
	void operator()(T* handle) const noexcept
	{
		CoTaskMemFree(handle);
	}
};

template<class T>
using unique_cotaskmem = std::unique_ptr<T, cotaskmem_deleter<T>>;

#endif

#include <chrono>
typedef std::ratio<1, 10000000> hn;
typedef std::chrono::duration<long long, hn> hnseconds;

#endif