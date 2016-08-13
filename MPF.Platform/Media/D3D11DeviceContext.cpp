//
// MPF Platform
// Direct3D 11 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-08-13
//
#include "stdafx.h"
#include "D3D11DeviceContext.h"
#include <map>
#include <process.h>
#include "../NativeApplication.h"
#include "resource.h"
#include "D3D11Vertex.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace concurrency;
using namespace D3D11;

namespace
{
	static const D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	void GetHardwareAdapter(IDXGIFactory* dxgiFactory, IDXGIAdapter** ppAdapter)
	{
		ComPtr<IDXGIAdapter> adapter;
		*ppAdapter = nullptr;

		for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters(adapterIndex, &adapter); adapterIndex++)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			if (SUCCEEDED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, FeatureLevels,
				_countof(FeatureLevels), D3D11_SDK_VERSION, nullptr, nullptr, nullptr)))
				break;
		}

		*ppAdapter = adapter.Detach();
	}

	void DumpAdapterInfo(IDXGIAdapter* adapter)
	{
		DXGI_ADAPTER_DESC desc;
		ThrowIfFailed(adapter->GetDesc(&desc));

		std::wstringstream ss;
		ss << "MPF Adapter Dump Info:";
		ss << std::endl << L"Adapter Information: ";
		ss << std::endl << L"Device Id: " << desc.DeviceId;
		ss << std::endl << L"Description: " << desc.Description;
		ss << std::endl << L"Dedicated Video Memory: " << desc.DedicatedVideoMemory << L" Bytes";
		ss << std::endl << L"Shared System Memory: " << desc.SharedSystemMemory << L" Bytes";

		_putws(ss.str().c_str());
	}

	void DumpFeatureLevel(D3D_FEATURE_LEVEL featureLevel)
	{
		const static std::map<D3D_FEATURE_LEVEL, const wchar_t*> map =
		{
			{ D3D_FEATURE_LEVEL_9_1	 , L"9.1" },
			{ D3D_FEATURE_LEVEL_9_2	 , L"9.2" } ,
			{ D3D_FEATURE_LEVEL_9_3	 , L"9.3" } ,
			{ D3D_FEATURE_LEVEL_10_0 , L"10.0" } ,
			{ D3D_FEATURE_LEVEL_10_1 , L"10.1" } ,
			{ D3D_FEATURE_LEVEL_11_0 , L"11.0" } ,
			{ D3D_FEATURE_LEVEL_11_1 , L"11.1" } ,
			{ D3D_FEATURE_LEVEL_12_0 , L"12.0" } ,
			{ D3D_FEATURE_LEVEL_12_1 , L"12.1" }
		};

		std::wstringstream ss;
		ss << L"Feature Level: ";
		auto it = map.find(featureLevel);
		if (it != map.end())
			ss << it->second;
		else
			ss << L"Unknown";
		_putws(ss.str().c_str());
	}
}

D3D11DeviceContext::D3D11DeviceContext(IDeviceContextCallback * callback)
	:_callback(callback), _renderObjectContainer(std::make_shared<RenderableObjectContainer<RenderableObject>>())
{
	ZeroMemory(&_swapChainUpdateContext, sizeof(_swapChainUpdateContext));
	ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&_dxgiFactory)));
	ComPtr<IDXGIAdapter> adapter;
	GetHardwareAdapter(_dxgiFactory.Get(), &adapter);

	UINT createFlags = 0
#if _DEBUG
		| D3D11_CREATE_DEVICE_DEBUG// | D3D11_CREATE_DEVICE_DEBUGGABLE
#endif
		;
	D3D_DRIVER_TYPE driverType;
	if (adapter)
	{
		driverType = D3D_DRIVER_TYPE_UNKNOWN;
		DumpAdapterInfo(adapter.Get());
	}
	else
	{
		driverType = D3D_DRIVER_TYPE_WARP;
		_putws(L"Using WARP Device.");
	}

	ThrowIfFailed(D3D11CreateDevice(adapter.Get(), driverType, nullptr, createFlags, FeatureLevels, _countof(FeatureLevels),
		D3D11_SDK_VERSION, &_device, &_featureLevel, &_deviceContext));
	DumpFeatureLevel(_featureLevel);

	ActiveDeviceAndStartRender();
}

D3D11DeviceContext::~D3D11DeviceContext()
{
	_deviceContext->Unmap(_wvpmCB.Get(), 0);
	ZeroMemory(&_swapChainUpdateContext, sizeof(_swapChainUpdateContext));
}

STDMETHODIMP D3D11DeviceContext::CreateSwapChain(INativeWindow * window, ISwapChain ** swapChain)
{
	try
	{
		*swapChain = Make<D3D11SwapChain>(window, _dxgiFactory.Get(), _deviceContext.Get()).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP D3D11DeviceContext::CreateRenderableObject(IRenderableObject ** obj)
{
	try
	{
		*obj = Make<RenderableObjectRef>(_renderObjectContainer, _renderObjectContainer->Allocate()).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP D3D11DeviceContext::CreateResourceManager(IResourceManager ** resMgr)
{
	return E_NOTIMPL;
}

namespace
{
	const std::pair<const DWORD*, DWORD> LoadShaderResource(int id)
	{
		auto hres = FindResourceW(ModuleHandle, MAKEINTRESOURCE(id), L"SHADER");
		ThrowWin32IfNot(hres);
		auto handle = LoadResource(ModuleHandle, hres);
		ThrowWin32IfNot(handle);
		auto size = SizeofResource(ModuleHandle, hres);
		ThrowWin32IfNot(size);
		auto ret = reinterpret_cast<const DWORD*>(LockResource(handle));
		ThrowWin32IfNot(ret);
		return{ ret, size };
	}
}

concurrency::task<void> D3D11DeviceContext::CreateDeviceResourcesAsync()
{
	auto uiVSData = LoadShaderResource(IDR_D3D11_UIVERTEXSHADER);
	static const D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(StrokeVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	ComPtr<ID3D11InputLayout> inputLayout;
	ThrowIfFailed(_device->CreateInputLayout(inputElementDesc, _countof(inputElementDesc), uiVSData.first,
		uiVSData.second, &inputLayout));

	ComPtr<ID3D11VertexShader> vertexShader;
	ThrowIfFailed(_device->CreateVertexShader(uiVSData.first, uiVSData.second, nullptr, &vertexShader));
	auto uiPSData = LoadShaderResource(IDR_D3D11_UIPIXELSHADER);
	ComPtr<ID3D11PixelShader> pixelShader;
	ThrowIfFailed(_device->CreatePixelShader(uiPSData.first, uiPSData.second, nullptr, &pixelShader));

	{
		D3D11_BUFFER_DESC desc{};
		static_assert(sizeof(WorldViewProjectionModelData) % 16 == 0, "Constant Buffer Byte Width Restriction.");
		desc.ByteWidth = sizeof(WorldViewProjectionModelData);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;

		ThrowIfFailed(_device->CreateBuffer(&desc, nullptr, &_wvpmCB));
		D3D11_MAPPED_SUBRESOURCE mapped;
		ThrowIfFailed(_deviceContext->Map(_wvpmCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));
		_swapChainUpdateContext.WVPMDataPointer = reinterpret_cast<WorldViewProjectionModelData*>(mapped.pData);
	}

	return task_from_result();
}

void D3D11DeviceContext::StartRenderLoop()
{
	if (!_isRenderLoopActive.load(std::memory_order_acquire))
	{
		auto handle = _beginthread(RenderLoop, 0, new WeakRef<D3D11DeviceContext>(GetWeakContext()));
		ThrowIfNot(handle != -1, L"Cannot create Render Loop Thread.");
		NativeApplication::AddEventToWait(reinterpret_cast<HANDLE>(handle));
	}
}

bool D3D11DeviceContext::IsActive() const noexcept
{
	return _isRenderLoopActive;
}

void D3D11DeviceContext::DoFrame()
{
	std::vector<ComPtr<D3D11SwapChain>> swapChains;
	{
		auto locker = _swapChainLock.Lock();
		for (auto&& weakRef : _swapChains)
			if (auto swapChain = weakRef.Resolve())
				swapChains.emplace_back(swapChain);
	}

	for (auto&& swapChain : swapChains)
		swapChain->Update();

	_callback->OnRender();
	UpdateResourceManagers();
	UpdateRenderObjects();

	for (auto&& swapChain : swapChains)
		swapChain->DoFrame(_swapChainUpdateContext);
}

void D3D11DeviceContext::DoFrameWrapper() noexcept
{
	auto hr = S_OK;
	try
	{
		DoFrame();
	}
	CATCH_ALL_WITHHR(hr);
}

void D3D11DeviceContext::UpdateRenderObjects()
{
}

void D3D11DeviceContext::UpdateResourceManagers()
{
	for (auto&& resMgrRef : _resourceManagers)
		if (auto resMgr = resMgrRef.Resolve())
			resMgr->Update();
}

void D3D11DeviceContext::ActiveDeviceAndStartRender()
{
	ComPtr<D3D11DeviceContext> thisHolder;
	CreateDeviceResourcesAsync().then([thisHolder, this]
	{
		this->StartRenderLoop();
	});
}

void D3D11DeviceContext::RenderLoop(void * weakRefVoid)
{
	std::unique_ptr<WeakRef<D3D11DeviceContext>> weakRef(reinterpret_cast<WeakRef<D3D11DeviceContext>*>(weakRefVoid));
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
