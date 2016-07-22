//
// MPF Platform
// Render Command Buffer
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#include "stdafx.h"
#include "RenderCommandBuffer.h"
#include "ResourceManager.h"
using namespace WRL;
using namespace NS_PLATFORM;

RenderCommandBuffer::RenderCommandBuffer(ResourceManager* resourceManager)
	:_resourceManager(resourceManager)
{
}

HRESULT RenderCommandBuffer::DrawGeometry(IResource * geometry)
{
	try
	{
		_geometries.emplace_back(static_cast<ResourceRef*>(geometry));
		return S_OK;
	}
	CATCH_ALL();
}
