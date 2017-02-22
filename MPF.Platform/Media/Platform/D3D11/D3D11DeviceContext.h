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
#include "../../DeviceContext.h"
#include "../../RenderableObjectContainer.h"
#include "../../ResourceManagerBase.h"
#include "../D3D11PlatformProvider.h"
#include <ppltasks.h>

DEFINE_NS_PLATFORM_D3D11

class D3D11DeviceContext : public WeakReferenceBase<D3D11DeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	D3D11DeviceContext(IDeviceContextCallback* callback);
	virtual ~D3D11DeviceContext();

	STDMETHODIMP CreateSwapChain(INativeWindow* window, ISwapChain** swapChain) override;
	STDMETHODIMP CreateRenderableObject(IRenderableObject ** obj) override;
	STDMETHODIMP CreateResourceManager(IResourceManager **resMgr) override;
	STDMETHODIMP Update() override;

	void SetPipelineState(PiplineStateTypes type);
	void SetPipelineState(PiplineStateTypes type, const MaterialRenderCall& mrc);

	DEFINE_PROPERTY_GET(Device, ID3D11Device*);
	ID3D11Device* get_Device() const noexcept { return _device.Get(); }
	DEFINE_PROPERTY_GET(DeviceContext, ID3D11DeviceContext*);
	ID3D11DeviceContext* get_DeviceContext() const noexcept { return _deviceContext.Get(); }
	DEFINE_PROPERTY_GET(UpdateContext, SwapChainUpdateContext&);
	SwapChainUpdateContext& get_UpdateContext() noexcept { return _swapChainUpdateContext; }
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
	SwapChainUpdateContext _swapChainUpdateContext;
	D3D_FEATURE_LEVEL _featureLevel;
	std::atomic<bool> _isRenderLoopActive = false;
	WRL::Wrappers::CriticalSection _swapChainLock;
	std::vector<WeakRef<D3D11SwapChain>> _swapChains;
	std::vector<WeakRef<ResourceManagerBase>> _resourceManagers;
	std::shared_ptr<RenderableObjectContainer<RenderableObject>> _renderObjectContainer;
	PiplineStateTypes _pipelineStateType = PiplineStateTypes::None;

	WRL::ComPtr<ID3D11InputLayout> _strokeInputLayout;
	WRL::ComPtr<ID3D11VertexShader> _strokeVS;
	WRL::ComPtr<ID3D11PixelShader> _strokePS;
	WRL::ComPtr<ID3D11GeometryShader> _strokeGS;

	WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState;
	WRL::ComPtr<ID3D11DepthStencilState> _depthStencilState3D;

	WRL::ComPtr<ID3D11InputLayout> _fillInputLayout;
	WRL::ComPtr<ID3D11VertexShader> _fillVS;
	WRL::ComPtr<ID3D11PixelShader> _fillPS;
};

END_NS_PLATFORM_D3D11