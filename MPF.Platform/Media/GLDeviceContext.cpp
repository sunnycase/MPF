//
// MPF Platform
// OpenGL DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#include "stdafx.h"
#include "GLDeviceContext.h"
#include "GLSwapChain.h"
#include "RenderableObject.h"
using namespace WRL;
using namespace NS_PLATFORM;

GLDeviceContext::GLDeviceContext(IDeviceContextCallback* callback)
	:_callback(callback), _renderObjectContainer(std::make_shared<RenderableObjectContainer<RenderableObject>>())
{

}

GLDeviceContext::~GLDeviceContext()
{
}

STDMETHODIMP GLDeviceContext::CreateSwapChain(INativeWindow * window, ISwapChain ** swapChain)
{
	try
	{
		*swapChain = Make<GLSwapChainBase>(window).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP GLDeviceContext::CreateRenderableObject(IRenderableObject ** obj)
{
	try
	{
		*obj = Make<RenderableObjectRef>(_renderObjectContainer, _renderObjectContainer->Allocate()).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP GLDeviceContext::CreateResourceManager(IResourceManager ** resMgr)
{
	return E_NOTIMPL;
}
