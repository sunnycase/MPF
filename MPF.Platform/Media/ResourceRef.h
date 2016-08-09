//
// MPF Platform
// Resource Ref
// 作者：SunnyCase
// 创建时间：2016-07-20
//
#pragma once
#include "../../inc/common.h"
#include "../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class ResourceManagerBase;

class ResourceRef : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResource>
{
public:
	ResourceRef(WeakRef<ResourceManagerBase>&& resMgr, ResourceType resType, UINT_PTR handle);

	STDMETHOD_(ULONG, Release)();

	ResourceType GetType() const noexcept { return _resType; }
	UINT_PTR GetHandle() const noexcept { return _handle; }
private:
	WeakRef<ResourceManagerBase> _resMgr;
	ResourceType _resType;
	UINT_PTR _handle;
};

END_NS_PLATFORM