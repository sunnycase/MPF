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

#define CTOR_IMPL1(T) \
_container##T(Make<ResourceContainer<T>>())

ResourceManagerBase::ResourceManagerBase()
	:CTOR_IMPL1(LineGeometry), CTOR_IMPL1(SolidColorBrush), CTOR_IMPL1(Pen)
{
}

#define CREATERESOURCE_IMPL1(T) 	   \
case RT_##T:						   \
container = _container##T.Get();	   \
handle = container->Allocate();		   \
_added##T.emplace_back(handle);		   \
break;								   

HRESULT ResourceManagerBase::CreateResource(ResourceType resType, IResource ** res)
{
	try
	{
		UINT_PTR handle;
		IResourceContainer* container = nullptr;
		switch (resType)
		{
			CREATERESOURCE_IMPL1(LineGeometry);
			CREATERESOURCE_IMPL1(SolidColorBrush);
			CREATERESOURCE_IMPL1(Pen);
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

#define UPDATE_RES_IMPL1_PRE(T)											\
try																		\
{																		\
	auto handle = static_cast<ResourceRef*>(res)->GetHandle();			\
	assert(static_cast<ResourceRef*>(res)->GetType() == RT_##T);			\
	auto& resObj = _container##T->FindResource(handle);

#define UPDATE_RES_IMPL1_AFT(T)											\
_updated##T.emplace_back(handle);										\
{																			   \
	auto& ddcls = resObj.GetDependentDrawCallLists(); \
	for (auto it = ddcls.begin(); it != ddcls.end(); ++it)					   \
	{																		   \
		if (auto dcl = it->lock())											   \
			_updatedDrawCallList.emplace_back(std::move(dcl));				   \
		else																   \
			it = ddcls.erase(it);											   \
	}																		   \
}																			   \
return S_OK;															\
}																		\
CATCH_ALL()

HRESULT ResourceManagerBase::UpdateLineGeometry(IResource * res, LineGeometryData * data)
{
	UPDATE_RES_IMPL1_PRE(LineGeometry)
		resObj.Data = *data;
	UPDATE_RES_IMPL1_AFT(LineGeometry);
}

HRESULT ResourceManagerBase::UpdateSolidColorBrush(IResource * res, ColorF * color)
{
	UPDATE_RES_IMPL1_PRE(SolidColorBrush)
		resObj.Color = *color;
	UPDATE_RES_IMPL1_AFT(SolidColorBrush);
}

HRESULT ResourceManagerBase::UpdatePen(IResource * res, float thickness, IResource * brush)
{
	UPDATE_RES_IMPL1_PRE(Pen)
		resObj.Thickness = thickness;
	resObj.Brush = static_cast<ResourceRef*>(brush);
	UPDATE_RES_IMPL1_AFT(Pen);
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
	return _containerLineGeometry->FindResource(handle);
}

Brush & ResourceManagerBase::GetBrush(UINT_PTR handle)
{
	return _containerSolidColorBrush->FindResource(handle);
}

const Brush & ResourceManagerBase::GetBrush(UINT_PTR handle) const
{
	return _containerSolidColorBrush->FindResource(handle);
}

LineGeometry& ResourceManagerBase::GetLineGeometry(UINT_PTR handle)
{
	return _containerLineGeometry->FindResource(handle);
}

const Pen& ResourceManagerBase::GetPen(UINT_PTR handle) const
{
	return _containerPen->FindResource(handle);
}

Pen& ResourceManagerBase::GetPen(UINT_PTR handle)
{
	return _containerPen->FindResource(handle);
}

#define UPDATE_IMPL1(T)			 \
_added##T.clear();				 \
_updated##T.clear();			 \
_container##T->CleanUp();		 

void ResourceManagerBase::Update()
{
	{
		auto& lineGeometryTRC = GetLineGeometryTRC();
		lineGeometryTRC.Add(_addedLineGeometry, *_containerLineGeometry.Get());
		lineGeometryTRC.Update(_updatedLineGeometry, *_containerLineGeometry.Get());
		lineGeometryTRC.Remove(_containerLineGeometry->GetCleanupList());
		UPDATE_IMPL1(LineGeometry);
	}
	{
		UPDATE_IMPL1(SolidColorBrush);
	}
	{
		UPDATE_IMPL1(Pen);
	}
	{
		for (auto&& dcl : _updatedDrawCallList)
			dcl->Update();
		_updatedDrawCallList.clear();
	}
	UpdateOverride();
}

#define ADDCL_IMPL1(T)															\
	case RT_##T:																\
		Get##T(resRef->GetHandle()).AddDependentDrawCallList(std::move(dcl));	\
		break;

void ResourceManagerBase::AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl, IResource* res)
{
	if (!res) return;
	auto resRef = static_cast<ResourceRef*>(res);
	switch (resRef->GetType())
	{
		ADDCL_IMPL1(LineGeometry);
		ADDCL_IMPL1(Pen);
	default:
		break;
	}
}