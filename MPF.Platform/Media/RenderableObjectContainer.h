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

struct IRenderableObjectContainer
{
	virtual RenderableObject& FindObject(UINT_PTR handle) = 0;
};

class RenderableObjectRef : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IRenderableObject>
{
public:
	RenderableObjectRef(std::shared_ptr<IRenderableObjectContainer>&& container, UINT_PTR handle);

	STDMETHOD_(ULONG, Release)();
	STDMETHODIMP SetOffset(float x, float y);
	STDMETHODIMP SetContent(IRenderCommandBuffer* buffer);
	STDMETHODIMP Render();
private:
	std::shared_ptr<IRenderableObjectContainer> _container;
	UINT_PTR _handle;
};

template<typename T>
class RenderableObjectContainer : public IRenderableObjectContainer, public ResourceContainer<T>
{
public:

	virtual RenderableObject& FindObject(UINT_PTR handle) override
	{
		return _data[handle].GetObject();
	}

	void Update()
	{
		for (auto&& obj : _data)
		{
			if (obj.GetUsed())
				obj.GetObject().Update();
		}
	}
};

END_NS_PLATFORM