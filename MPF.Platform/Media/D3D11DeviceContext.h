//
// MPF Platform
// Direct3D 11 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-08-13
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include "D3D11SwapChain.h"
#include <dxgi.h>
#include <d3d11_1.h>
#include <atomic>
#include "DeviceContext.h"
#include "RenderableObjectContainer.h"
#include "ResourceManagerBase.h"
#include <ppltasks.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D11DeviceContext : public WeakReferenceBase<D3D11DeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	D3D11DeviceContext(IDeviceContextCallback* callback);
	virtual ~D3D11DeviceContext();

	STDMETHODIMP CreateSwapChain(INativeWindow* window, ISwapChain** swapChain) override;
	STDMETHODIMP CreateRenderableObject(IRenderableObject ** obj) override;
	STDMETHODIMP CreateResourceManager(IResourceManager **resMgr);
private:
	concurrency::task<void> CreateDeviceResourcesAsync();
	void StartRenderLoop();
	bool IsActive() const noexcept;
	void DoFrame();
	void DoFrameWrapper() noexcept;
	void UpdateRenderObjects();
	void UpdateResourceManagers();
	void ActiveDeviceAndStartRender();
	static void __cdecl RenderLoop(void* weakRefVoid);
private:
	WRL::ComPtr<IDeviceContextCallback> _callback;
	WRL::ComPtr<IDXGIFactory> _dxgiFactory;
	WRL::ComPtr<ID3D11Device> _device;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext;
	SwapChainUpdateContext _swapChainUpdateContext = {};
	D3D_FEATURE_LEVEL _featureLevel;
	std::atomic<bool> _isRenderLoopActive = false;
	WRL::Wrappers::CriticalSection _swapChainLock;
	std::vector<WeakRef<D3D11SwapChain>> _swapChains;
	std::vector<WeakRef<ResourceManagerBase>> _resourceManagers;
	std::shared_ptr<RenderableObjectContainer<RenderableObject>> _renderObjectContainer;
};

END_NS_PLATFORM