//
// MPF Platform
// Direct3D 9 SwapChain
// 作者：SunnyCase
// 创建时间：2016-07-18
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include <d3d9.h>
#include <atomic>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct DECLSPEC_UUID("A0DCDAC4-9F8F-4BA3-8DF5-E32AFD05C851") ID3D9SwapChain : public ISwapChain
{
	virtual void DoFrame() = 0;
};

class D3D9ChildSwapChain : public WeakReferenceBase<D3D9ChildSwapChain, WRL::RuntimeClassFlags<WRL::ClassicCom>, ISwapChain, ID3D9SwapChain>
{
public:
	D3D9ChildSwapChain(IDirect3DSwapChain9* swapChain, IDirect3DDevice9* device, INativeWindow* window);

	virtual void DoFrame() override;
private:
	HWND _hWnd;
	WRL::ComPtr<IDirect3DSwapChain9> _swapChain;
	WRL::ComPtr<IDirect3DDevice9> _device;
};

class D3D9SwapChain : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, ISwapChain, ID3D9SwapChain>
{
public:
	D3D9SwapChain(IDirect3D9* d3d, INativeWindow* window);

	IDirect3DDevice9* GetDevice() const noexcept
	{
		return _device.Get();
	}

	void CreateAdditionalSwapChain(INativeWindow* window, D3D9ChildSwapChain** swapChain);

	virtual void DoFrame() override;
private:
	D3DPRESENT_PARAMETERS CreatePresentParameters(HWND hWnd) const noexcept;
	void CreateDeviceResource(IDirect3D9* d3d);
private:
	HWND _hWnd;
	D3DCAPS9 _deviceCaps = {};
	D3DFORMAT _backBufferFormat = D3DFMT_UNKNOWN;
	WRL::ComPtr<IDirect3DDevice9> _device;
};
END_NS_PLATFORM