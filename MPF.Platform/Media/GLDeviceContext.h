//
// MPF Platform
// OpenGL DeviceContext
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/WeakReferenceBase.h"
#include "DeviceContext.h"
#include "RenderableObjectContainer.h"
DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class GLDeviceContext : public WeakReferenceBase<GLDeviceContext, WRL::RuntimeClassFlags<WRL::ClassicCom>, IDeviceContext>
{
public:
	GLDeviceContext(DeviceContextMessagesHandler messageHandler);
	virtual ~GLDeviceContext();

	STDMETHODIMP CreateSwapChain(INativeWindow* window, ISwapChain** swapChain) override;
	STDMETHODIMP CreateRenderableObject(IRenderableObject ** obj) override;
	STDMETHODIMP CreateResourceManager(IResourceManager **resMgr);
private:
	DeviceContextMessagesHandler _messageHandler;
	std::shared_ptr<RenderableObjectContainer<RenderableObject>> _renderObjectContainer;
};

END_NS_PLATFORM