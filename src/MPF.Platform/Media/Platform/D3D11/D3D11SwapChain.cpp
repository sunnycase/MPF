//
// MPF Platform
// Direct3D 11 SwapChain
// 作者：SunnyCase
// 创建时间：2016-08-13
//
#include "stdafx.h"
#include "D3D11SwapChain.h"
#include "../../ResourceRef.h"
#include "../../ResourceManagerBase.h"
#include <DirectXMath.h>
#include <stack>
#include <array>
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
		D3D11_TEXTURE2D_DESC dsDesc{};
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.Width = _viewport.Width;
		dsDesc.Height = _viewport.Height;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.CPUAccessFlags = 0;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.MiscFlags = 0;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;

		ThrowIfFailed(device->CreateTexture2D(&dsDesc, nullptr, &_depthStencilBuffer));

		ThrowIfFailed(device->CreateDepthStencilView(_depthStencilBuffer.Get(), 
			&CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT), &_depthStencilView));
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

struct SwapChainDrawingContext : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ISwapChainDrawingContext>
{
	SwapChainDrawingContext(SwapChainUpdateContext& context, ID3D11DeviceContext* deviceContext)
		:_context(context), _deviceContext(deviceContext)
	{

	}

	// 通过 RuntimeClass 继承
	STDMETHODIMP PushCamera(MPF::Platform::IResource *camera)
	{
		ARGUMENT_NOTNULL_HR(camera);
		try
		{
			auto& cameraRef = *static_cast<ResourceRef*>(camera);
			auto resMgr = cameraRef.GetResourceManagerWeak().Resolve();
			if (resMgr)
			{
				Camera cameraObj;
				resMgr->TryGetCamera(camera, cameraObj);
				PushAndApply({ cameraObj.View, cameraObj.Projection });
				return S_OK;
			}
			return E_NOT_VALID_STATE;
		}
		CATCH_ALL();
	}

	STDMETHODIMP PopCamera()
	{
		try
		{
			if (!_cameras.empty())
			{
				_cameras.pop();
				if (!_cameras.empty())
				{
					Apply(_cameras.top());
					return S_OK;
				}
			}
			return E_NOT_VALID_STATE;
		}
		CATCH_ALL();
	}

	STDMETHODIMP PushViewport(Rect *viewport)
	{
		ARGUMENT_NOTNULL_HR(viewport);
		try
		{
			PushAndApply({ viewport->Left, viewport->Top, viewport->Width, viewport->Height, 0.f, 1.f });
			return S_OK;
		}
		CATCH_ALL();
	}

	STDMETHODIMP PopViewport()
	{
		try
		{
			if (!_viewports.empty())
			{
				_viewports.pop();
				if (!_viewports.empty())
				{
					Apply(_viewports.top());
					return S_OK;
				}
			}
			return E_NOT_VALID_STATE;
		}
		CATCH_ALL();
	}

	void PushAndApply(const std::array<DirectX::XMFLOAT4X4, 2>& camera)
	{
		_cameras.push(camera);
		Apply(camera);
	}

	void PushAndApply(const D3D11_VIEWPORT& viewport)
	{
		_viewports.push(viewport);
		Apply(viewport);
	}

	std::stack<std::array<DirectX::XMFLOAT4X4, 2>> _cameras;
	std::stack<D3D11_VIEWPORT> _viewports;
private:
	void Apply(const std::array<DirectX::XMFLOAT4X4, 2>& camera)
	{
		auto wvp = _context.WVP.Map(_deviceContext.Get());
		wvp->WorldView = camera[0];
		wvp->Projection = camera[1];
	}

	void Apply(const D3D11_VIEWPORT& viewport)
	{
		_deviceContext->RSSetViewports(1, &viewport);
	}

	SwapChainUpdateContext& _context;
	ComPtr<ID3D11DeviceContext> _deviceContext;
};

void D3D11SwapChain::DoFrame(SwapChainUpdateContext& context)
{
	if (_needResize)
	{
		OnResize();
		_needResize = false;
	}

	auto drawingContext = Make<SwapChainDrawingContext>(context, _deviceContext.Get());
	drawingContext->PushAndApply({ _wvp.WorldView, _wvp.Projection });

	ID3D11RenderTargetView* const renderTargetViews[] = { _renderTargetView.Get() };
	XMVECTORF32 color = { 1.f, 1.f, 1.f, 1.f };

	drawingContext->PushAndApply(_viewport);
	_deviceContext->OMSetRenderTargets(1, renderTargetViews, _depthStencilView.Get());
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), color.f);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);

	if (auto callback = _callback)
		callback->OnDraw(drawingContext.Get());

	ThrowIfFailed(_swapChain->Present(1, 0));
}