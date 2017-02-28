//
// MPF Platform
// OpenGL SwapChain
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class GLSwapChainBase : public WeakReferenceBase<GLSwapChainBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, ISwapChain>
{
public:
	GLSwapChainBase(INativeWindow* window);
	virtual ~GLSwapChainBase();

	// 通过 WeakReferenceBase 继承
	STDMETHODIMP SetCallback(ISwapChainCallback * callback) override;
private:
	HWND _hWnd;
	HDC _hdc;
	HGLRC _glCtx;
	WRL::ComPtr<ISwapChainCallback> _callback;
};

END_NS_PLATFORM