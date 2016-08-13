//
// MPF Platform
// Resource Ref
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#include "stdafx.h"
#include "ResourceRef.h"
#include "ResourceContainer.h"
#include "ResourceManagerBase.h"
using namespace WRL;
using namespace NS_PLATFORM;

ResourceRef::ResourceRef(NS_PLATFORM::WeakRef<ResourceManagerBase>&& resMgr, ResourceType resType, UINT_PTR handle)
	:_resMgr(std::move(resMgr)), _handle(handle), _resType(resType)
{

}

STDMETHODIMP_(ULONG) ResourceRef::Release()
{
	ULONG ref = InternalRelease();
	if (ref == 0)
	{
		if (auto resMgr = _resMgr.Resolve())
			resMgr->RetireResource(this);
		delete this;

		auto module = ::Microsoft::WRL::GetModuleBase();
		if (module != nullptr)
		{
			module->DecrementObjectCount();
		}
	}

	return ref;
}
