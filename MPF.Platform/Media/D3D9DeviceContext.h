//
// MPF Platform
// Direct3D 9 DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-16
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include <d3d9.h>
#include <atomic>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9DeviceContext : public WeakReferenceBase<D3D9DeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	D3D9DeviceContext(INativeWindow* nativeWindow);
private:
	void CreateDeviceResources();
	void StartRenderLoop();
	bool IsActive() const noexcept;
	void DoFrame();
	void DoFrameWrapper() noexcept;
	static void RenderLoop(void* weakRefVoid);
private:
	HWND _hWnd;
	WRL::ComPtr<IDirect3D9> _d3d;
	WRL::ComPtr<IDirect3DDevice9> _device;
	D3DCAPS9 _deviceCaps = {};
	D3DFORMAT _backBufferFormat = D3DFMT_UNKNOWN;
	std::atomic<bool> _isRenderLoopActive = false;
};

END_NS_PLATFORM