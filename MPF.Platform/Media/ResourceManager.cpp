//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#include "stdafx.h"
#include "ResourceManager.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"
using namespace WRL;

HRESULT __stdcall CreateResourceManager(NS_PLATFORM::ResourceManager** obj) noexcept
{
	try
	{
		*obj = Make<NS_PLATFORM::ResourceManager>().Detach();
		return S_OK;
	}
	CATCH_ALL();
}

using namespace NS_PLATFORM;

ResourceManager::ResourceManager()
	:_lineGeometryContainer(Make<ResourceContainer<LineGeometry>>())
{
}

HRESULT ResourceManager::CreateResource(ResourceType resType, IResource ** res)
{
	try
	{
		IResourceContainer* container = nullptr;
		switch (resType)
		{
		case RT_LineGeometry:
			container = _lineGeometryContainer.Get();
			break;
		default:
			break;
		}
		if (container)
		{
			*res = Make<ResourceRef>(container, resType, container->Allocate()).Detach();
			return S_OK;
		}
		return E_INVALIDARG;
	}
	CATCH_ALL();
}

HRESULT ResourceManager::UpdateLineGeometry(IResource * res, LineGeometryData * data)
{
	return E_NOTIMPL;
}

HRESULT ResourceManager::CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer)
{
	try
	{
		*buffer = Make<RenderCommandBuffer>(this).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

const LineGeometry& ResourceManager::GetLineGeometry(UINT_PTR handle) const
{
	return _lineGeometryContainer->FindResource(handle);
}