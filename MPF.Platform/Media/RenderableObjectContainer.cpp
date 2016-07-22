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

RenderableObjectRef::RenderableObjectRef(IRenderableObjectContainer * container, UINT_PTR handle)
	:_container(container), _handle(handle)
{

}

STDMETHODIMP_(ULONG) RenderableObjectRef::Release()
{
	ULONG ref = InternalRelease();
	if (ref == 0)
	{
		ComPtr<IResourceContainer> resContainer;
		if(SUCCEEDED(_container.As(&resContainer)))
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
