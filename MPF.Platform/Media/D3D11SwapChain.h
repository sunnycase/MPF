//
// MPF Platform
// Direct3D 11 SwapChain
// 作者：SunnyCase
// 创建时间：2016-08-13
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include <dxgi.h>
#include <d3d11.h>
#include "D3D11Vertex.h"
#include "../Controls/NativeWindow.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct SwapChainUpdateContext
{
	D3D11::WorldViewProjectionModelData* WVPMDataPointer;
};

class D3D11SwapChain : public WeakReferenceBase<D3D11SwapChain, WRL::RuntimeClassFlags<WRL::ClassicCom>, ISwapChain>
{
public:
	D3D11SwapChain(INativeWindow* window, IDXGIFactory* dxgiFactory, ID3D11DeviceContext* deviceContext);
	virtual ~D3D11SwapChain();

	// 通过 WeakReferenceBase 继承
	STDMETHODIMP SetCallback(ISwapChainCallback * callback) override;
	void DoFrame(SwapChainUpdateContext& context);
	void Update();
private:
	void CreateWindowSizeDependentResources();
	void UpdateShaderConstants(SwapChainUpdateContext& context);
	void OnNativeWindowMessage(NativeWindowMessages message);
	void OnResize();
private:
	HWND _hWnd;
	WRL::ComPtr<ISwapChainCallback> _callback;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext;
	WRL::ComPtr<IDXGISwapChain> _swapChain;
	WRL::ComPtr<ID3D11RenderTargetView> _renderTargetView;
	WRL::ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView;
	D3D11_VIEWPORT _viewport;
	D3D11::WorldViewProjectionModelData _wvp;
};

END_NS_PLATFORM