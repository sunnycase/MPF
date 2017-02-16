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

struct Geometry3DRenderCommand
{
	WRL::ComPtr<ResourceRef> Geometry3D;
	WRL::ComPtr<ResourceRef> Material;
	DirectX::XMFLOAT4X4 Transform;
};

class RenderCommandBuffer : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IRenderCommandBuffer>
{
public:
	RenderCommandBuffer(ResourceManagerBase* resourceManager);

	// 通过 RuntimeClass 继承
	STDMETHODIMP DrawGeometry(IResource * geometry, IResource* pen, IResource* brush, float* values) override;
	STDMETHODIMP DrawGeometry3D(IResource * geometry, IResource* material, float* values) override;

	const std::vector<GeometryRenderCommand>& GetCommands() const noexcept { return _commands; }
	const std::vector<Geometry3DRenderCommand>& GetCommand3Ds() const noexcept { return _command3Ds; }
	ResourceManagerBase* GetResourceManager() const noexcept { return _resourceManager.Get(); }
private:
	WRL::ComPtr<ResourceManagerBase> _resourceManager;
	std::vector<GeometryRenderCommand> _commands;
	std::vector<Geometry3DRenderCommand> _command3Ds;
};
END_NS_PLATFORM