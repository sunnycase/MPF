//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#include "stdafx.h"
#include "D3D9DeviceContext.h"
#include <process.h>
#include "D3D9ResourceManager.h"
#include "D3D9Vertex.h"
#include "../resource.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace D3D;
using namespace concurrency;

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
			_device = d3dSwapChain->GetDevice();
			_rootSwapChain = d3dSwapChain;
			ThrowIfFailed(d3dSwapChain.CopyTo(swapChain));
			ActiveDeviceAndStartRender();
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

namespace
{
	const DWORD* LoadShaderResource(int id)
	{
		auto hres = FindResourceW(ModuleHandle, MAKEINTRESOURCE(id), L"SHADER");
		ThrowWin32IfNot(hres);
		auto handle = LoadResource(ModuleHandle, hres);
		ThrowWin32IfNot(handle);
		auto ret = reinterpret_cast<const DWORD*>(LockResource(handle));
		ThrowWin32IfNot(ret);
		return ret;
	}
}

task<void> D3D9DeviceContext::CreateDeviceResourcesAsync()
{
	ComPtr<IDirect3DVertexDeclaration9> vertexDeclaration;
	static const D3DVERTEXELEMENT9 elements[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, offsetof(Vertex, Color), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		D3DDECL_END()
	};
	ThrowIfFailed(_device->CreateVertexDeclaration(elements, &vertexDeclaration));
	ThrowIfFailed(_device->SetVertexDeclaration(vertexDeclaration.Get()));

	auto uiVSData = LoadShaderResource(IDR_UIVERTEXSHADER);
	ComPtr<IDirect3DVertexShader9> vs;
	ThrowIfFailed(_device->CreateVertexShader(uiVSData, &vs));

	auto uiPSData = LoadShaderResource(IDR_UIPIXELSHADER);
	ComPtr<IDirect3DPixelShader9> ps;
	ThrowIfFailed(_device->CreatePixelShader(uiPSData, &ps));

	ThrowIfFailed(_device->SetVertexShader(vs.Get()));
	ThrowIfFailed(_device->SetPixelShader(ps.Get()));

	return task_from_result();
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
	UpdateResourceManagers();
	UpdateRenderObjects();

	std::vector<ComPtr<D3D9SwapChainBase>> swapChains;
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

void D3D9DeviceContext::UpdateResourceManagers()
{
	for (auto&& resMgrRef : _resourceManagers)
		if (auto resMgr = resMgrRef.Resolve())
			resMgr->Update();
}

void D3D9DeviceContext::ActiveDeviceAndStartRender()
{
	ComPtr<D3D9DeviceContext> thisHolder;
	CreateDeviceResourcesAsync().then([thisHolder, this]
	{
		this->StartRenderLoop();
	});
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

HRESULT D3D9DeviceContext::CreateResourceManager(IResourceManager **resMgr)
{
	try
	{
		auto myResMgr = Make<D3D9ResourceManager>(_device.Get());
		_resourceManagers.emplace_back(myResMgr->GetWeakContext());
		*resMgr = myResMgr.Detach();
		return S_OK;
	}
	CATCH_ALL();
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
