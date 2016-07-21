//
// MPF Platform
// Resource Ref
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#pragma once
#include "../../inc/common.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class ResourceRef : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResource>
{
public:
	ResourceRef(IResourceContainer* container, UINT_PTR handle)
		:_container(container), _handle(handle)
	{

	}

	STDMETHOD_(ULONG, Release)()
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
private:
	WRL::ComPtr<IResourceContainer> _container;
	UINT_PTR _handle;
};

END_NS_PLATFORM