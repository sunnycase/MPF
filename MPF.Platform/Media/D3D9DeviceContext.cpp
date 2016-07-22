//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "D3D9DeviceContext.h"
#include <process.h>
using namespace WRL;
using namespace NS_PLATFORM;

D3D9DeviceContext::D3D9DeviceContext(DeviceContextMessagesHandler messageHandler)
	:_rootSwapChainLock(5000), _messageHandler(messageHandler), _renderObjectContainer(Make<RenderableObjectContainer<D3D9RenderableObject>>())
{
	_d3d.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	ThrowIfNot(_d3d, L"Cannot Initialize Direct3D 9 Interface.");
}

STDMETHODIMP D3D9DeviceContext::CreateSwapChain(INativeWindow * window, ISwapChain ** swapChain)
{
	try
	{
		auto locker = _rootSwapChainLock.Lock();
		if (!_rootSwapChain)
		{
			auto d3dSwapChain = Make<D3D9SwapChain>(_d3d.Get(), window);
			_rootSwapChain = d3dSwapChain;
			ThrowIfFailed(d3dSwapChain.CopyTo(swapChain));
			StartRenderLoop();
		}
		else
		{
			ComPtr<D3D9ChildSwapChain> d3dSwapChain;
			_rootSwapChain->CreateAdditionalSwapChain(window, &d3dSwapChain);
			_childSwapChains.emplace_back(d3dSwapChain->GetWeakContext());
			ThrowIfFailed(d3dSwapChain.CopyTo(swapChain));
		}
		return S_OK;
	}
	CATCH_ALL();
}

void D3D9DeviceContext::CreateDeviceResources()
{

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
	_messageHandler(DeviceContextMessages::DCM_Render);
	UpdateRenderObjects();

	std::vector<ComPtr<ID3D9SwapChain>> swapChains;
	{
		auto locker = _rootSwapChainLock.Lock();
		if (_rootSwapChain)
			swapChains.emplace_back(_rootSwapChain);
		for (auto&& weakRef : _childSwapChains)
			if (auto swapChain = weakRef.Resolve())
				swapChains.emplace_back(swapChain);
	}
	for (auto&& swapChain : swapChains)
		swapChain->DoFrame();
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

void D3D9DeviceContext::UpdateRenderObjects()
{
	_renderObjectContainer->Update();
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

HRESULT D3D9DeviceContext::CreateRenderableObject(IRenderableObject ** obj)
{
	try
	{
		_renderObjectContainer->AllocateObjectRef(obj);
		return S_OK;
	}
	CATCH_ALL();
}
