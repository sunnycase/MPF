//
// MPF Platform
// Renderable Object Container
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#include "stdafx.h"
#include "RenderableObjectContainer.h"
using namespace WRL;
using namespace NS_PLATFORM;

RenderableObjectRef::RenderableObjectRef(std::shared_ptr<IRenderableObjectContainer>&& container, UINT_PTR handle)
	:_container(std::move(container)), _handle(handle)
{

}

STDMETHODIMP RenderableObjectRef::SetParent(IRenderableObject* parent)
{
	try
	{
		auto& childObj = _container->FindObject(_handle);
		auto parentObj = parent ? &_container->FindObject(static_cast<RenderableObjectRef*>(parent)->_handle) : nullptr;
		childObj.SetParent(parentObj);
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP_(ULONG) RenderableObjectRef::Release()
{
	ULONG ref = InternalRelease();
	if (ref == 0)
	{
		auto container(_container);
		if(auto resContainer = dynamic_cast<IResourceContainer*>(container.get()))
			resContainer->RetireResource(_handle);
		delete this;

		auto module = ::Microsoft::WRL::GetModuleBase();
		if (module != nullptr)
		{
			module->DecrementObjectCount();
		}
	}

	return ref;
}

STDMETHODIMP RenderableObjectRef::SetContent(IRenderCommandBuffer * buffer)
{
	try
	{
		_container->FindObject(_handle).SetContent(buffer);
		return S_OK;
	}
	CATCH_ALL();
}


STDMETHODIMP RenderableObjectRef::Render()
{
	try
	{
		_container->FindObject(_handle).Draw();
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP RenderableObjectRef::SetOffset(float x, float y)
{
	try
	{
		_container->FindObject(_handle).SetOffset(x, y);
		return S_OK;
	}
	CATCH_ALL();
}