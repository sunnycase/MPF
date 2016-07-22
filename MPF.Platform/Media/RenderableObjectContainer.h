//
// MPF Platform
// Renderable Object Container
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "RenderableObject.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct DECLSPEC_UUID("193F6B97-9D78-409F-BF78-8B528BCF709E") IRenderableObjectContainer : public IUnknown
{
	virtual RenderableObject& FindObject(UINT_PTR handle) = 0;
};

class RenderableObjectRef : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IRenderableObject>
{
public:
	RenderableObjectRef(IRenderableObjectContainer* container, UINT_PTR handle);

	STDMETHOD_(ULONG, Release)();
	STDMETHODIMP SetContent(IRenderCommandBuffer* buffer);
private:
	WRL::ComPtr<IRenderableObjectContainer> _container;
	UINT_PTR _handle;
};

template<typename T>
class RenderableObjectContainer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, ResourceContainerImplement<T>, IRenderableObjectContainer>
{
public:

	void AllocateObjectRef(IRenderableObject** obj)
	{
		*obj = WRL::Make<RenderableObjectRef>(this, Allocate()).Detach();
	}

	virtual RenderableObject& FindObject(UINT_PTR handle) override
	{
		return _data[handle];
	}

	void Update()
	{
		for (auto&& obj : _data)
		{
			if (obj.Used)
				obj.Update();
		}
	}
};

END_NS_PLATFORM