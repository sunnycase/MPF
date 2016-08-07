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
#include "D3D9Vertex.h"
#include <DirectXMath.h>
#include "../Controls/NativeWindow.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9SwapChainBase : public WeakReferenceBase<D3D9SwapChainBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, ISwapChain>
{
public:
	D3D9SwapChainBase(INativeWindow* window);

	STDMETHODIMP SetCallback(ISwapChainCallback* callback) override;
	virtual void DoFrame() = 0;
	void Update();

protected:
	void CreateWindowSizeDependentResources();
	void UpdateShaderConstants();
	void SetDevice(IDirect3DDevice9* device) { _device = device; }
	void Draw(IDirect3DSurface9* surface);
	D3DPRESENT_PARAMETERS CreatePresentParameters(HWND hWnd) const noexcept;
	virtual void OnReset() = 0;
	virtual void OnResize();
private:
	void OnNativeWindowMessage(NativeWindowMessages message);
protected:
	D3DFORMAT _backBufferFormat = D3DFMT_UNKNOWN;
	HWND _hWnd;
	D3D::WorldViewProjectionData _wvp;
	D3DVIEWPORT9 _viewport;
	WRL::ComPtr<IDirect3DDevice9> _device;
	WRL::ComPtr<ISwapChainCallback> _callback;
	bool _needResize = false;
};

class D3D9ChildSwapChain : public D3D9SwapChainBase
{
public:
	D3D9ChildSwapChain(IDirect3DSwapChain9* swapChain, IDirect3DDevice9* device, INativeWindow* window);

	virtual void DoFrame() override;
protected:
	virtual void OnReset() override;
private:
	WRL::ComPtr<IDirect3DSwapChain9> _swapChain;
};

class D3D9SwapChain : public D3D9SwapChainBase
{
public:
	D3D9SwapChain(IDirect3D9* d3d, INativeWindow* window);

	IDirect3DDevice9* GetDevice() const noexcept
	{
		return _device.Get();
	}

	void CreateAdditionalSwapChain(INativeWindow* window, D3D9ChildSwapChain** swapChain);

	virtual void DoFrame() override;

	bool NeedReset = false;
	void Reset() { OnReset(); }
protected:
	virtual void OnReset() override;
	virtual void OnResize() override;
private:
	void CreateDeviceResource(IDirect3D9* d3d);
private:
	D3DCAPS9 _deviceCaps = {};
};
END_NS_PLATFORM