//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include "D3D9SwapChain.h"
#include <d3d9.h>
#include <atomic>
#include "DeviceContext.h"
#include "D3D9RenderableObject.h"
#include "RenderableObjectContainer.h"
#include "ResourceManagerBase.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9DeviceContext : public WeakReferenceBase<D3D9DeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	D3D9DeviceContext(DeviceContextMessagesHandler messageHandler);

	STDMETHODIMP CreateSwapChain(INativeWindow* window, ISwapChain** swapChain) override;
	STDMETHODIMP CreateRenderableObject(IRenderableObject ** obj) override;
	STDMETHODIMP CreateResourceManager(IResourceManager **resMgr);
private:
	void CreateDeviceResources();
	void StartRenderLoop();
	bool IsActive() const noexcept;
	void DoFrame();
	void DoFrameWrapper() noexcept;
	void UpdateRenderObjects();
	void UpdateResourceManagers();
	static void __cdecl RenderLoop(void* weakRefVoid);
private:
	DeviceContextMessagesHandler _messageHandler;
	WRL::ComPtr<IDirect3D9> _d3d;
	WRL::ComPtr<IDirect3DDevice9> _device;
	std::atomic<bool> _isRenderLoopActive = false;
	WRL::Wrappers::CriticalSection _rootSwapChainLock;
	WRL::ComPtr<D3D9SwapChain> _rootSwapChain;
	std::vector<WeakRef<D3D9SwapChainBase>> _childSwapChains;
	std::vector<WeakRef<ResourceManagerBase>> _resourceManagers;
	WRL::ComPtr<RenderableObjectContainer<D3D9RenderableObject>> _renderObjectContainer;
};

END_NS_PLATFORM