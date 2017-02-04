//
// MPF Platform
// Direct3D 11 SwapChain
// 作者：SunnyCase
// 创建时间：2016-08-13
//
#include "stdafx.h"
#include "D3D11SwapChain.h"
#include <DirectXMath.h>
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D11;
using namespace DirectX;

namespace
{
	HWND GetNativeHandle(INativeWindow* nativeWindow)
	{
		INT_PTR handle;
		ThrowIfFailed(nativeWindow->get_NativeHandle(&handle));
		return reinterpret_cast<HWND>(handle);
	}
}

D3D11SwapChain::D3D11SwapChain(INativeWindow * window, IDXGIFactory* dxgiFactory, ID3D11DeviceContext* deviceContext)
	:_hWnd(GetNativeHandle(window)), _deviceContext(deviceContext)
{
	ThrowIfFailed(dxgiFactory->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER));
	{
		ComPtr<ID3D11Device> device;
		deviceContext->GetDevice(&device);
		DXGI_SWAP_CHAIN_DESC desc{};
		desc.OutputWindow = _hWnd;
		desc.Windowed = TRUE;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 3;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.RefreshRate = { 60, 1 };
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		ThrowIfFailed(dxgiFactory->CreateSwapChain(device.Get(), &desc, &_swapChain));

		CreateWindowSizeDependentResources();
	}
	ComPtr<INativeWindowIntern> windowIntern;
	ThrowIfFailed(window->QueryInterface(IID_PPV_ARGS(&windowIntern)));
	windowIntern->AppendMessageHandler([weakRef = AsWeak()](NativeWindowMessages message)
	{
		if (auto me = weakRef.Resolve())
			me->OnNativeWindowMessage(message);
	});
}

D3D11SwapChain::~D3D11SwapChain()
{
}

STDMETHODIMP D3D11SwapChain::SetCallback(ISwapChainCallback * callback)
{
	try
	{
		_callback = callback;
		return S_OK;
	}
	CATCH_ALL();
}

void D3D11SwapChain::OnNativeWindowMessage(NativeWindowMessages message)
{
	switch (message)
	{
	case MPF::Platform::NWM_Closing:
		break;
	case MPF::Platform::NWM_Closed:
		break;
	case MPF::Platform::NWM_SizeChanged:
		_needResize = true;
		break;
	default:
		break;
	}
}

void D3D11SwapChain::CreateWindowSizeDependentResources()
{
	RECT viewRect;
	ThrowWin32IfNot(GetClientRect(_hWnd, &viewRect));
	_viewport = { 0, 0, static_cast<float>(viewRect.right - viewRect.left), static_cast<float>(viewRect.bottom - viewRect.top), 0.f, 1.f };

	XMMATRIX projection = XMMatrixOrthographicOffCenterLH(0, float(_viewport.Width), float(_viewport.Height), 0, 0.f, 1.f);

	XMStoreFloat4x4(&_wvp.WorldView, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&_wvp.Projection, XMMatrixTranspose(projection));

	ComPtr<ID3D11Device> device;
	_deviceContext->GetDevice(&device);
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &_renderTargetView));

	{
		//D3D11_TEXTURE2D_DESC dsDesc{};
		//dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		//dsDesc.Width = 640;
		//dsDesc.Height = 480;
		//dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//dsDesc.MipLevels = 1;
		//dsDesc.ArraySize = 1;
		//dsDesc.CPUAccessFlags = 0;
		//dsDesc.SampleDesc.Count = 1;
		//dsDesc.SampleDesc.Quality = 0;
		//dsDesc.MiscFlags = 0;
		//dsDesc.Usage = D3D11_USAGE_DEFAULT;

		//ThrowIfFailed(device->CreateTexture2D(&dsDesc, nullptr, &_depthStencilBuffer));
		//
	}
}

void D3D11SwapChain::OnResize()
{
	RECT viewRect;
	ThrowWin32IfNot(GetClientRect(_hWnd, &viewRect));

	if (viewRect.right - viewRect.left <= 0 ||
		viewRect.bottom - viewRect.top <= 0) return;

	_renderTargetView.Reset();
	ThrowIfFailed(_swapChain->ResizeBuffers(3, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
	CreateWindowSizeDependentResources();
}

void D3D11SwapChain::UpdateShaderConstants(SwapChainUpdateContext& context)
{
	auto wvp = context.WVP.Map(_deviceContext.Get());
	wvp->WorldView = _wvp.WorldView;
	wvp->Projection = _wvp.Projection;
}

void D3D11SwapChain::DoFrame(SwapChainUpdateContext& context)
{
	if (_needResize)
	{
		OnResize();
		_needResize = false;
	}

	UpdateShaderConstants(context);

	ID3D11RenderTargetView* const renderTargetViews[] = { _renderTargetView.Get() };
	XMVECTORF32 color = { 1.f, 1.f, 1.f, 1.f };

	_deviceContext->RSSetViewports(1, &_viewport);
	_deviceContext->OMSetRenderTargets(1, renderTargetViews, nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), color.f);

	if (auto callback = _callback)
		callback->OnDraw();

	ThrowIfFailed(_swapChain->Present(1, 0));
}