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
#include "../../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

template<typename T>
struct ITransformedResourceContainer
{
	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Remove(const std::vector<UINT_PTR>& handles) = 0;
};

class ResourceManagerBase : public WeakReferenceBase<ResourceManagerBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceManager>
{
public:
	ResourceManagerBase();

	// 通过 RuntimeClass 继承
	STDMETHODIMP CreateResource(ResourceType resType, IResource ** res) override;
	STDMETHODIMP UpdateLineGeometry(IResource * res, LineGeometryData * data) override;
	STDMETHODIMP CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer) override;

	const LineGeometry& GetLineGeometry(UINT_PTR handle) const;
	void Update();
protected:
	virtual ITransformedResourceContainer<LineGeometry>& GetLineGeometryTRC() noexcept = 0;
	virtual void UpdateOverride() {};
private:
	WRL::ComPtr<ResourceContainer<LineGeometry>> _lineGeometryContainer;
	std::vector<UINT_PTR> _addedLineGeometry;
	std::vector<UINT_PTR> _updatedLineGeometry;
};
END_NS_PLATFORM