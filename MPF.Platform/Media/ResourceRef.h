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

struct IResourceContainer;

class ResourceRef : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResource>
{
public:
	ResourceRef(std::shared_ptr<IResourceContainer>&& container, ResourceType resType, UINT_PTR handle);

	STDMETHOD_(ULONG, Release)();

	ResourceType GetType() const noexcept { return _resType; }
	UINT_PTR GetHandle() const noexcept { return _handle; }
private:
	std::shared_ptr<IResourceContainer> _container;
	ResourceType _resType;
	UINT_PTR _handle;
};

END_NS_PLATFORM