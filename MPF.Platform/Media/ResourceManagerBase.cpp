//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#include "stdafx.h"
#include "ResourceManagerBase.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"
using namespace WRL;
using namespace NS_PLATFORM;

ResourceManagerBase::ResourceManagerBase()
	:_lineGeometryContainer(Make<ResourceContainer<LineGeometry>>())
{
}

HRESULT ResourceManagerBase::CreateResource(ResourceType resType, IResource ** res)
{
	try
	{
		UINT_PTR handle;
		IResourceContainer* container = nullptr;
		switch (resType)
		{
		case RT_LineGeometry:
			container = _lineGeometryContainer.Get();
			handle = container->Allocate();
			_addedLineGeometry.emplace_back(handle);
			break;
		default:
			break;
		}
		if (container)
		{
			*res = Make<ResourceRef>(container, resType, handle).Detach();
			return S_OK;
		}
		return E_INVALIDARG;
	}
	CATCH_ALL();
}

HRESULT ResourceManagerBase::UpdateLineGeometry(IResource * res, LineGeometryData * data)
{
	try
	{
		auto handle = static_cast<ResourceRef*>(res)->GetHandle();
		_lineGeometryContainer->FindResource(handle).Data = *data;
		_updatedLineGeometry.emplace_back(handle);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT ResourceManagerBase::CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer)
{
	try
	{
		*buffer = Make<RenderCommandBuffer>(this).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

const LineGeometry& ResourceManagerBase::GetLineGeometry(UINT_PTR handle) const
{
	return _lineGeometryContainer->FindResource(handle);
}

void ResourceManagerBase::Update()
{
	{
		auto& lineGeometryTRC = GetLineGeometryTRC();
		lineGeometryTRC.Add(_addedLineGeometry, *_lineGeometryContainer.Get());
		lineGeometryTRC.Update(_updatedLineGeometry, *_lineGeometryContainer.Get());
		lineGeometryTRC.Remove(_lineGeometryContainer->GetCleanupList());
		_addedLineGeometry.clear();
		_updatedLineGeometry.clear();
		_lineGeometryContainer->CleanUp();
	}
	UpdateOverride();
}
