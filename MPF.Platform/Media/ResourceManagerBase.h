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
#include "Camera.h"
#include "Material.h"
#include "Texture.h"
#include "Sampler.h"
#include "../../inc/WeakReferenceBase.h"
#include "RenderCommandBuffer.h"
#include "Platform/PlatformProvider.h"
#include <DirectXMath.h>
#include <unordered_map>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class FontManager;

#define DECL_GEOMETRY_RESOURCEMGR_TRC_GETTER(T)		  \
virtual ITransformedResourceContainer<T>& Get##T##StrokeTRC() noexcept = 0; \
virtual ITransformedResourceContainer<T>& Get##T##FillTRC() noexcept = 0

#define DECL_GEOMETRY3D_RESOURCEMGR_TRC_GETTER(T)		  \
virtual ITransformedResourceContainer<T>& Get##T##FillTRC() noexcept = 0;

#define DECL_DEVICE_RESOURCEMGR_TRC_GETTER(T)		  \
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept = 0


class ResourceManagerBase : public WeakReferenceBase<ResourceManagerBase, WRL::RuntimeClassFlags<WRL::ClassicCom>, IResourceManager>
{
public:
	ResourceManagerBase();

	// 通过 RuntimeClass 继承
	STDMETHODIMP CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer) override;
	STDMETHODIMP CreateFontFaceFromMemory(INT_PTR buffer, UINT64 size, UINT faceIndex, IFontFace **fontFace) override;
	STDMETHODIMP CreateResource(ResourceType resType, IResource ** res) override;
	STDMETHODIMP UpdateLineGeometry(IResource * res, LineGeometryData * data) override;
	STDMETHODIMP UpdateRectangleGeometry(IResource * res, RectangleGeometryData * data) override;
	HRESULT UpdatePathGeometry(IResource * res, std::vector<PathGeometrySegments::Segment>&& segments) noexcept;
	STDMETHODIMP UpdatePathGeometry(IResource * res, byte* data, UINT32 length) override;
	STDMETHODIMP UpdateBrush(IResource * res, IResource* texture, IResource* sampler) override;
	STDMETHODIMP UpdatePen(IResource * res, float thickness, IResource* brush) override;
	STDMETHODIMP UpdateCamera(IResource * res, float* viewMatrix, float* projectionMatrix) override;
	STDMETHODIMP UpdateShaderParameters(IResource * res, BYTE *data, UINT dataSize, IResource *brushes[], UINT brushesCount) override;
	STDMETHODIMP UpdateMaterial(IResource * res, IShadersGroup *shader, IResource* shaderParameters) override;
	STDMETHODIMP UpdateBoxGeometry3D(IResource * res, BoxGeometry3DData * data) override;
	STDMETHODIMP UpdateMeshGeometry3D(IResource * res, MeshGeometry3DData * data) override;
	STDMETHODIMP UpdateSolidColorTexture(IResource * res, ColorF * color) override;
	STDMETHODIMP UpdateSampler(IResource * res, SamplerData * data) override;

	void RetireResource(IResource * res);

	virtual void UpdateGPU();
	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) = 0;
	void AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl, IResource* res);

	void AddDependentResource(UINT_PTR handle, IResource* res);

	virtual void BeginResetDevice() {}
	virtual void EndResetDevice() {}

	virtual bool TryGetCamera(IResource* res, Camera& camera) const = 0;
protected:
	DECL_GEOMETRY_RESOURCEMGR_TRC_GETTER(LineGeometry);
	DECL_GEOMETRY_RESOURCEMGR_TRC_GETTER(RectangleGeometry);
	DECL_GEOMETRY_RESOURCEMGR_TRC_GETTER(PathGeometry);

	DECL_GEOMETRY3D_RESOURCEMGR_TRC_GETTER(BoxGeometry3D);
	DECL_GEOMETRY3D_RESOURCEMGR_TRC_GETTER(MeshGeometry3D);

	DECL_DEVICE_RESOURCEMGR_TRC_GETTER(SolidColorTexture);
	DECL_DEVICE_RESOURCEMGR_TRC_GETTER(Sampler);

	DECL_DEVICE_RESOURCEMGR_TRC_GETTER(Brush);
	DECL_DEVICE_RESOURCEMGR_TRC_GETTER(Pen);
private:
	std::vector<std::shared_ptr<IDrawCallList>> _updatedDrawCallList;
	std::unordered_multimap<UINT_PTR, std::weak_ptr<IDrawCallList>> _dependentDrawCallLists;
	std::unordered_multimap<UINT_PTR, UINT_PTR> _dependentResources;
	WRL::Wrappers::CriticalSection _containerCS;
	std::shared_ptr<FontManager> _fontManager;
	std::atomic<UINT_PTR> _nextResourceId = 1;
};
END_NS_PLATFORM