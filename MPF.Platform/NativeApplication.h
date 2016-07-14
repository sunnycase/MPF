//
// MPF Platform
// Native Application
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#pragma once
#include "../inc/common.h"
#include "../inc/NonCopyable.h"
#include "../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class NativeApplication : private NonCopyable,
	public WeakReferenceBase<NativeApplication, WRL::RuntimeClassFlags<WRL::ClassicCom>, INativeApplication>
{
public:
	NativeApplication();
	virtual ~NativeApplication();

	// 通过 WeakReferenceBase 继承
	STDMETHODIMP Run(void) override;
private:
private:
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateNativeApplication(NS_PLATFORM::INativeApplication** obj) noexcept;
}