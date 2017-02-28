//
// MPF Platform
// DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "DeviceContext.h"
#include "Platform/D3D9/D3D9DeviceContext.h"
#include "Platform/GL/GLDeviceContext.h"
#include "Platform/D3D11/D3D11DeviceContext.h"
using namespace WRL;

namespace
{
	template<class T>
	HRESULT CreateDeviceContext(NS_PLATFORM::IDeviceContextCallback* callback, NS_PLATFORM::IDeviceContext** obj) noexcept
	{
		try
		{
			*obj = Make<T>(callback).Detach();
			return S_OK;
		}
		CATCH_ALL();
	}

#define RETURN_IF_SUCCEEDED(hr) if(SUCCEEDED(hr)) return hr

	typedef HRESULT(*fnCreateDeviceContext)(NS_PLATFORM::IDeviceContextCallback*, NS_PLATFORM::IDeviceContext**);
	static fnCreateDeviceContext cdcFns[NS_PLATFORM::RBT_COUNT] =
	{
		nullptr,
		CreateDeviceContext<NS_PLATFORM::GLDeviceContext>,
		CreateDeviceContext<NS_PLATFORM_D3D9::D3D9DeviceContext>,
		CreateDeviceContext<NS_PLATFORM_D3D11::D3D11DeviceContext>,
	};
}

HRESULT __stdcall CreateDeviceContext(NS_PLATFORM::RenderBackendType preferredBackend, NS_PLATFORM::IDeviceContextCallback* callback, NS_PLATFORM::IDeviceContext** obj) noexcept
{
	auto hr = E_FAIL;
	if (preferredBackend != NS_PLATFORM::RBT_Any)
	{
		hr = cdcFns[preferredBackend](callback, obj);
		RETURN_IF_SUCCEEDED(hr);
	}
	for (size_t i = 1; i < _countof(cdcFns); ++i)
	{
		if (i == preferredBackend)continue;
		hr = cdcFns[i](callback, obj);
		RETURN_IF_SUCCEEDED(hr);
	}
	return hr;
}