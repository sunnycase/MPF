//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#include "stdafx.h"
#include "ResourceManager.h"
#include "ResourceRef.h"
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
		HRESULT hr = S_OK;
		switch (resType)
		{
		case RT_LineGeometry:
			*res = Make<ResourceRef>(_lineGeometryContainer.Get(), _lineGeometryContainer->Allocate()).Detach();
			break;
		default:
			hr = E_INVALIDARG;
			break;
		}
		return hr;
	}
	CATCH_ALL();
}

HRESULT ResourceManager::UpdateLineGeometry(IResource * res, LineGeometryData * data)
{
	return E_NOTIMPL;
}