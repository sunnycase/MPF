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
#include "DirectXMath.h"

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
	virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) = 0;
	virtual void Update() = 0;
};

#define DECL_RESCONTAINERAWARE(T) \
std::shared_ptr<ResourceContainer<T>> _container##T; \
std::vector<UINT_PTR> _added##T;				 \
std::vector<UINT_PTR> _updated##T;					 

#define DECL_RESOURCEMGR_IMPL1(T)		  \
T& Get##T##(UINT_PTR handle);			  \
const T& Get##T##(UINT_PTR handle) const;

#define DEFINE_RESOURCEMGR_IMPL1(T)															  \
T& Get##T##(UINT_PTR handle) { return _container##T##->FindResource(handle); }				  \
const T& Get##T##(UINT_PTR handle) const { return _container##T##->FindResource(handle); }

class ResourceManagerBase : public WeakReferenceBase<ResourceManagerBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceManager>
{
public:
	ResourceManagerBase();

	// 通过 RuntimeClass 继承
	STDMETHODIMP CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer) override;
	STDMETHODIMP CreateResource(ResourceType resType, IResource ** res) override;
	STDMETHODIMP UpdateLineGeometry(IResource * res, LineGeometryData * data) override;
	STDMETHODIMP UpdateRectangleGeometry(IResource * res, RectangleGeometryData * data) override;
	STDMETHODIMP UpdatePathGeometry(IResource * res, byte* data, UINT32 length) override;
	STDMETHODIMP UpdateSolidColorBrush(IResource * res, ColorF * color) override;
	STDMETHODIMP UpdatePen(IResource * res, float thickness, IResource* brush) override;

	DEFINE_RESOURCEMGR_IMPL1(LineGeometry);
	DEFINE_RESOURCEMGR_IMPL1(RectangleGeometry);
	DEFINE_RESOURCEMGR_IMPL1(PathGeometry);
	DECL_RESOURCEMGR_IMPL1(Brush);
	DEFINE_RESOURCEMGR_IMPL1(Pen);

	void Update();
	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) = 0;
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl, IResource* res);

	virtual void BeginResetDevice() {}
	virtual void EndResetDevice() {}
protected:
	virtual ITransformedResourceContainer<LineGeometry>& GetLineGeometryTRC() noexcept = 0;
	virtual ITransformedResourceContainer<RectangleGeometry>& GetRectangleGeometryTRC() noexcept = 0;
	virtual ITransformedResourceContainer<PathGeometry>& GetPathGeometryTRC() noexcept = 0;
	virtual void UpdateOverride() {};
private:
	DECL_RESCONTAINERAWARE(LineGeometry);
	DECL_RESCONTAINERAWARE(RectangleGeometry);
	DECL_RESCONTAINERAWARE(PathGeometry);
	DECL_RESCONTAINERAWARE(SolidColorBrush);
	DECL_RESCONTAINERAWARE(Pen);
	std::vector<std::shared_ptr<IDrawCallList>> _updatedDrawCallList;
};
END_NS_PLATFORM