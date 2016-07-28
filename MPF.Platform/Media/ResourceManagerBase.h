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
#include "Brush.h"
#include "Pen.h"
#include "../../inc/WeakReferenceBase.h"
#include "RenderCommandBuffer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

template<typename T>
struct ITransformedResourceContainer
{
	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) = 0;
	virtual void Remove(const std::vector<UINT_PTR>& handles) = 0;
};

struct IDrawCallList : std::enable_shared_from_this<IDrawCallList>
{
	virtual void Draw() = 0;
	virtual void Update() = 0;
};

#define DECL_RESCONTAINERAWARE(T) \
WRL::ComPtr<ResourceContainer<T>> _container##T; \
std::vector<UINT_PTR> _added##T;				 \
std::vector<UINT_PTR> _updated##T;					 

class ResourceManagerBase : public WeakReferenceBase<ResourceManagerBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceManager>
{
public:
	ResourceManagerBase();

	// 通过 RuntimeClass 继承
	STDMETHODIMP CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer) override;
	STDMETHODIMP CreateResource(ResourceType resType, IResource ** res) override;
	STDMETHODIMP UpdateLineGeometry(IResource * res, LineGeometryData * data) override;
	STDMETHODIMP UpdateSolidColorBrush(IResource * res, ColorF * color) override;
	STDMETHODIMP UpdatePen(IResource * res, float thickness, IResource* brush) override;

	LineGeometry& GetLineGeometry(UINT_PTR handle);
	const LineGeometry& GetLineGeometry(UINT_PTR handle) const;
	Brush& GetBrush(UINT_PTR handle);
	const Brush& GetBrush(UINT_PTR handle) const;
	Pen& GetPen(UINT_PTR handle);
	const Pen& GetPen(UINT_PTR handle) const;
	void Update();
	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) = 0;
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl, IResource* res);
protected:
	virtual ITransformedResourceContainer<LineGeometry>& GetLineGeometryTRC() noexcept = 0;
	virtual void UpdateOverride() {};
private:
	DECL_RESCONTAINERAWARE(LineGeometry);
	DECL_RESCONTAINERAWARE(SolidColorBrush);
	DECL_RESCONTAINERAWARE(Pen);
	std::vector<std::shared_ptr<IDrawCallList>> _updatedDrawCallList;
};
END_NS_PLATFORM