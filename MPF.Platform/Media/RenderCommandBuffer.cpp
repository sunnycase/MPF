//
// MPF Platform
// Render Command Buffer
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#include "stdafx.h"
#include "RenderCommandBuffer.h"
#include "ResourceManagerBase.h"
using namespace WRL;
using namespace NS_PLATFORM;

RenderCommandBuffer::RenderCommandBuffer(ResourceManagerBase* resourceManager)
	:_resourceManager(resourceManager)
{
}

HRESULT RenderCommandBuffer::DrawGeometry(IResource * geometry, IResource* pen, float* values)
{
	try
	{
		GeometryRenderCommand command{ static_cast<ResourceRef*>(geometry), static_cast<ResourceRef*>(pen) };
		DirectX::XMStoreFloat4x4(&command.Transform, DirectX::XMMatrixTranspose(DirectX::XMMATRIX(values)));
		_geometries.emplace_back(std::move(command));
		return S_OK;
	}
	CATCH_ALL();
}
