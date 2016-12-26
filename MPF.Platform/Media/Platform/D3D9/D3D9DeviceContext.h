//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#pragma once
#include "../inc/WeakReferenceBase.h"
#include "../D3D9PlatformProvider.h"
#include "../../RenderableObjectContainer.h"
#include "D3D9SwapChain.h"
#include <ppltasks.h>

DEFINE_NS_PLATFORM_D3D9

class D3D9DeviceContext : public WeakReferenceBase<D3D9DeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	D3D9DeviceContext(IDeviceContextCallback* callback);
	virtual ~D3D9DeviceContext();

	STDMETHODIMP CreateSwapChain(INativeWindow* window, ISwapChain** swapChain) override;
	STDMETHODIMP CreateRenderableObject(IRenderableObject ** obj) override;
	STDMETHODIMP CreateResourceManager(IResourceManager **resMgr);

	DEFINE_PROPERTY_GET(D3D, IDirect3D9*);
	IDirect3D9* get_D3D() const noexcept { return _d3d.Get(); }
	DEFINE_PROPERTY_GET(Device, IDirect3DDevice9*);
	IDirect3DDevice9* get_Device() const noexcept { return _device.Get(); }
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
	void BeginResetDevice();
	void EndResetDevice();
	void EnsureDevice();
private:
	WRL::ComPtr<INativeWindow> _dummyWindow;
	WRL::ComPtr<IDeviceContextCallback> _callback;
	WRL::ComPtr<IDirect3D9> _d3d;
	WRL::ComPtr<IDirect3DDevice9> _device;
	std::atomic<bool> _isRenderLoopActive = false;
	WRL::Wrappers::CriticalSection _rootSwapChainLock;
	WRL::ComPtr<D3D9SwapChain> _rootSwapChain;
	std::vector<WeakRef<D3D9SwapChainBase>> _childSwapChains;
	std::vector<WeakRef<ResourceManagerBase>> _resourceManagers;
	std::shared_ptr<RenderableObjectContainer<RenderableObject>> _renderObjectContainer;
};

END_NS_PLATFORM_D3D9