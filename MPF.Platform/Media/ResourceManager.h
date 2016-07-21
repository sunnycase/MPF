//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "Geometry.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class ResourceManager : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceManager>
{
public:
	ResourceManager();

	// 通过 RuntimeClass 继承
	STDMETHODIMP CreateResource(ResourceType resType, IResource ** res) override;
	STDMETHODIMP UpdateLineGeometry(IResource * res, LineGeometryData * data) override;
private:
	WRL::ComPtr<ResourceContainer<LineGeometry>> _lineGeometryContainer;
};
END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateResourceManager(NS_PLATFORM::ResourceManager** obj) noexcept;
}