//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "D3D9DeviceContext.h"
#include <algorithm>
#include <array>
#include <process.h>
using namespace WRL;
using namespace NS_PLATFORM;

namespace
{
	UINT SelectHardwareAdapter(IDirect3D9* d3d)
	{
		std::vector<UINT> validAdapters;
		auto adapterCount = d3d->GetAdapterCount();
		for (UINT i = 0; i < adapterCount; i++)
		{
			D3DDISPLAYMODE displayMode;
			if (FAILED(d3d->GetAdapterDisplayMode(i, &displayMode))) continue;
			D3DCAPS9 caps{};
			if (FAILED(d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &caps)))continue;
			validAdapters.emplace_back(i);
		}
		if (validAdapters.empty())
			ThrowIfFailed(CO_E_NOT_SUPPORTED);
		return validAdapters.front();
	}

	void DumpAdapter(IDirect3D9* d3d, IDirect3DDevice9* device, UINT adapter, const D3DCAPS9& caps) noexcept
	{
		std::stringstream ss;
		D3DADAPTER_IDENTIFIER9 identifier;
		if (SUCCEEDED(d3d->GetAdapterIdentifier(adapter, 0, &identifier)))
		{
			ss << "MPF Adapter Dump Info:\n"
				<< "Adapter (" << adapter << "): \n"
				<< "Id: " << identifier.DeviceId << '\n'
				<< "Name: " << identifier.DeviceName << '\n'
				<< "Description: " << identifier.Description << '\n'
				<< "Driver: " << identifier.Driver << ", Version: "
				<< identifier.DriverVersion.HighPart << '.' << identifier.DriverVersion.LowPart << '\n'
				<< "Vertex Shader Version: " << D3DSHADER_VERSION_MAJOR(caps.VertexShaderVersion) << '.' << D3DSHADER_VERSION_MINOR(caps.VertexShaderVersion) << '\n'
				<< "Pixel Shader Version: " << D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion) << '.' << D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion) << '\n'
				<< "Texture Memory Size: " << device->GetAvailableTextureMem() << " Bytes." << std::endl;
			puts(ss.str().c_str());
		}
	}

	HWND GetNativeHandle(INativeWindow* nativeWindow)
	{
		INT_PTR handle;
		ThrowIfFailed(nativeWindow->get_NativeHandle(&handle));
		return reinterpret_cast<HWND>(handle);
	}

	HRESULT TryCreateDevice(UINT adapter, IDirect3D9* d3d, HWND hWnd, D3DPRESENT_PARAMETERS& params, IDirect3DDevice9** device, DWORD behaviorFlags
#if _DEBUG
		, const wchar_t info[]
#endif
	)
	{
		auto hr = d3d->CreateDevice(adapter, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, device);
#if _DEBUG
		if (SUCCEEDED(hr))
			_putws(info);
#endif
		return hr;
	}
}

D3D9DeviceContext::D3D9DeviceContext(INativeWindow* nativeWindow)
	:_hWnd(GetNativeHandle(nativeWindow))
{
	_d3d.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	ThrowIfNot(_d3d, L"Cannot Initialize Direct3D 9 Interface.");
	CreateDeviceResources();
	StartRenderLoop();
}

void D3D9DeviceContext::CreateDeviceResources()
{
	auto adapter = SelectHardwareAdapter(_d3d.Get());
	ThrowIfFailed(_d3d->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &_deviceCaps));
	D3DPRESENT_PARAMETERS params{};
	params.BackBufferCount = std::min(D3DPRESENT_BACK_BUFFERS_MAX, 3L);
	params.BackBufferFormat = _backBufferFormat;
	params.hDeviceWindow = _hWnd;
	params.Windowed = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	const std::array<DWORD, 3> behaviorFlags = { D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_MIXED_VERTEXPROCESSING , D3DCREATE_SOFTWARE_VERTEXPROCESSING };
#if _DEBUG
	const std::array<const wchar_t*, 3> infos = { L"Using Hardware Vertex Processing.",
		L"Using Mixed Vertex Processing." , L"Using Software Vertex Processing." };
	static_assert(behaviorFlags.size() == infos.size(), "behaviorFlags.size() must be equal to infos.size().");
#endif
	auto hr = S_OK;
	for (size_t i = 0; i < behaviorFlags.size(); i++)
	{
		hr = TryCreateDevice(adapter, _d3d.Get(), _hWnd, params, &_device, behaviorFlags[i]
#if _DEBUG
			, infos[i]
#endif
		);
		if (SUCCEEDED(hr))break;
	}
	ThrowIfFailed(hr);
#if _DEBUG
	DumpAdapter(_d3d.Get(), _device.Get(), adapter, _deviceCaps);
#endif
}

void D3D9DeviceContext::StartRenderLoop()
{
	if (!_isRenderLoopActive.load(std::memory_order_acquire))
	{
		ThrowIfNot(_beginthread(RenderLoop, 0, new WeakRef<D3D9DeviceContext>(GetWeakContext())) != -1,
			L"Cannot create Render Loop Thread.");
	}
}

bool D3D9DeviceContext::IsActive() const noexcept
{
	return true;
}

void D3D9DeviceContext::DoFrame()
{
	ThrowIfFailed(_device->Clear(0, nullptr, D3DCLEAR_TARGET, 0x0000FF, 1.f, 0));
	ThrowIfFailed(_device->BeginScene());
	ThrowIfFailed(_device->EndScene());
	auto hr = _device->Present(nullptr, nullptr, nullptr, nullptr);
	if (hr == D3DERR_DEVICELOST)
		;
	else
		ThrowIfFailed(hr);
}

void D3D9DeviceContext::DoFrameWrapper() noexcept
{
	auto hr = S_OK;
	try
	{
		DoFrame();
	}
	CATCH_ALL_WITHHR(hr);
}

void D3D9DeviceContext::RenderLoop(void * weakRefVoid)
{
	std::unique_ptr<WeakRef<D3D9DeviceContext>> weakRef(reinterpret_cast<WeakRef<D3D9DeviceContext>*>(weakRefVoid));
	if (auto me = weakRef->Resolve())
		if (me->_isRenderLoopActive.exchange(true))return;

	while (true)
	{
		if (auto me = weakRef->Resolve())
		{
			if (!me->IsActive())
			{
				me->_isRenderLoopActive.store(false, std::memory_order_release);
				break;
			}
			me->DoFrameWrapper();
		}
	}
}
