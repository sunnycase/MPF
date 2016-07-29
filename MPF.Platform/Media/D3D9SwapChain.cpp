//
// MPF Platform
// Direct3D 9 SwapChain
// 作者：SunnyCase
// 创建时间：2016-07-18
//
#include "stdafx.h"
#include "D3D9SwapChain.h"
#include <algorithm>
#include <array>
#include "D3D9Vertex.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;

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

D3D9ChildSwapChain::D3D9ChildSwapChain(IDirect3DSwapChain9 * swapChain, IDirect3DDevice9* device, INativeWindow * window)
	:D3D9SwapChainBase(window), _swapChain(swapChain)
{
	SetDevice(device);
}

void D3D9ChildSwapChain::DoFrame()
{
	ComPtr<IDirect3DSurface9> backSurface;
	ThrowIfFailed(_swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backSurface));
	Draw(backSurface.Get());
}

D3D9SwapChain::D3D9SwapChain(IDirect3D9 * d3d, INativeWindow * window)
	: D3D9SwapChainBase(window)
{
	CreateDeviceResource(d3d);
}

void D3D9SwapChain::CreateAdditionalSwapChain(INativeWindow * window, D3D9ChildSwapChain ** swapChain)
{
	auto params = CreatePresentParameters(GetNativeHandle(window));
	ComPtr<IDirect3DSwapChain9> d3dSwapChain;
	ThrowIfFailed(_device->CreateAdditionalSwapChain(&params, &d3dSwapChain));
	*swapChain = Make<D3D9ChildSwapChain>(d3dSwapChain.Get(), _device.Get(), window).Detach();
}

void D3D9SwapChain::DoFrame()
{
	ComPtr<IDirect3DSurface9> backSurface;
	ThrowIfFailed(_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backSurface));
	Draw(backSurface.Get());
}

D3DPRESENT_PARAMETERS D3D9SwapChain::CreatePresentParameters(HWND hWnd) const noexcept
{
	D3DPRESENT_PARAMETERS params{};
	params.BackBufferCount = std::min(D3DPRESENT_BACK_BUFFERS_MAX, 2L);
	params.BackBufferFormat = _backBufferFormat;
	params.hDeviceWindow = hWnd;
	params.Windowed = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	return params;
}

void D3D9SwapChain::CreateDeviceResource(IDirect3D9 * d3d)
{
	auto adapter = SelectHardwareAdapter(d3d);
	ThrowIfFailed(d3d->GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &_deviceCaps));
	auto params = CreatePresentParameters(_hWnd);

	const std::array<DWORD, 3> behaviorFlags = { D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_MIXED_VERTEXPROCESSING , D3DCREATE_SOFTWARE_VERTEXPROCESSING };
#if _DEBUG
	const std::array<const wchar_t*, 3> infos = { L"Using Hardware Vertex Processing.",
		L"Using Mixed Vertex Processing." , L"Using Software Vertex Processing." };
	static_assert(behaviorFlags.size() == infos.size(), "behaviorFlags.size() must be equal to infos.size().");
#endif
	auto hr = S_OK;
	ComPtr<IDirect3DDevice9> device;
	for (size_t i = 0; i < behaviorFlags.size(); i++)
	{
		hr = TryCreateDevice(adapter, d3d, _hWnd, params, &device, behaviorFlags[i]
#if _DEBUG
			, infos[i]
#endif
		);
		if (SUCCEEDED(hr)) break;
	}
	ThrowIfFailed(hr);
#if _DEBUG
	DumpAdapter(d3d, device.Get(), adapter, _deviceCaps);
#endif
	SetDevice(device.Get());
}

D3D9SwapChainBase::D3D9SwapChainBase(INativeWindow* window)
	:_hWnd(GetNativeHandle(window))
{
	CreateWindowSizeDependentResources();
}

HRESULT D3D9SwapChainBase::SetCallback(ISwapChainCallback * callback)
{
	try
	{
		_callback = callback;
		return S_OK;
	}
	CATCH_ALL();
}

void D3D9SwapChainBase::CreateWindowSizeDependentResources()
{
	RECT viewRect;
	ThrowWin32IfNot(GetClientRect(_hWnd, &viewRect));
	_viewport = { 0, 0, static_cast<DWORD>(viewRect.right - viewRect.left), static_cast<DWORD>(viewRect.bottom - viewRect.top), 0.f, 1.f };

	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0, float(_viewport.Width), float(_viewport.Height), 0, 0.f, 1.f);

	XMStoreFloat4x4(&_wvp.WorldView, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&_wvp.Projection, XMMatrixTranspose(projection));
}

void D3D9SwapChainBase::UpdateShaderConstants()
{
	ThrowIfFailed(_device->SetVertexShaderConstantF(D3D::VSCSlot_WorldViewProjection, reinterpret_cast<const float*>(&_wvp), D3D::VSCSize_WorldViewProjection));
	ThrowIfFailed(_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
}

void D3D9SwapChainBase::Draw(IDirect3DSurface9 * surface)
{
	UpdateShaderConstants();
	ThrowIfFailed(_device->SetViewport(&_viewport));
	ThrowIfFailed(_device->SetRenderTarget(0, surface));
	ThrowIfFailed(_device->BeginScene());
	ThrowIfFailed(_device->Clear(0, nullptr, D3DCLEAR_TARGET, 0xFFFFFF, 1.f, 0));

	if (auto callback = _callback)
		callback->OnDraw();

	ThrowIfFailed(_device->EndScene());
	ThrowIfFailed(_device->Present(nullptr, nullptr, nullptr, nullptr));
}
