//
// MPF Platform
// Resource Ref
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#include "stdafx.h"
#include "ResourceRef.h"
#include "ResourceContainer.h"
using namespace WRL;
using namespace NS_PLATFORM;

ResourceRef::ResourceRef(IResourceContainer * container, ResourceType resType, UINT_PTR handle)
	:_container(container), _handle(handle), _resType(resType)
{

}

STDMETHODIMP_(ULONG) ResourceRef::Release()
{
	ULONG ref = InternalRelease();
	if (ref == 0)
	{
		_container->RetireResource(_handle);
		delete this;

		auto module = ::Microsoft::WRL::GetModuleBase();
		if (module != nullptr)
		{
			module->DecrementObjectCount();
		}
	}

	return ref;
}
