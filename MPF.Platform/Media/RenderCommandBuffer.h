//
// MPF Platform
// Render Command Buffer
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "Geometry.h"
#include "ResourceRef.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class ResourceManagerBase;

struct GeometryRenderCommand
{
	WRL::ComPtr<ResourceRef> Geometry;
	WRL::ComPtr<ResourceRef> Pen;
	WRL::ComPtr<ResourceRef> Brush;
	DirectX::XMFLOAT4X4 Transform;
};

class RenderCommandBuffer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IRenderCommandBuffer>
{
public:
	RenderCommandBuffer(ResourceManagerBase* resourceManager);

	// 通过 RuntimeClass 继承
	STDMETHODIMP DrawGeometry(IResource * geometry, IResource* pen, IResource* brush, float* values) override;

	const std::vector<GeometryRenderCommand>& GetGeometries() const noexcept { return _geometries; }
	ResourceManagerBase* GetResourceManager() const noexcept { return _resourceManager.Get(); }
private:
	WRL::ComPtr<ResourceManagerBase> _resourceManager;
	std::vector<GeometryRenderCommand> _geometries;
};
END_NS_PLATFORM