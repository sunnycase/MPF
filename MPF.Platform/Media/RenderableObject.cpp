//
// MPF Platform
// Renderable Object
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#include "stdafx.h"
#include "RenderableObject.h"
#include "ResourceManagerBase.h"
using namespace WRL;
using namespace NS_PLATFORM;

RenderableObject::RenderableObject()
	:_isDirty(true), _isBufferDirty(true)
{
}

void RenderableObject::SetContent(IRenderCommandBuffer * buffer)
{
	auto myBuffer = static_cast<RenderCommandBuffer*>(buffer);
	if (myBuffer != _buffer.Get())
	{
		_buffer = myBuffer;
		SetBufferDirty();
	}
}

void RenderableObject::Update()
{
	if (_isDirty && _buffer)
	{
		auto resMgr = _buffer->GetResourceManager();
		for (auto&& geoRef : _buffer->GetGeometries())
		{
			auto handle = geoRef->GetHandle();
			switch (geoRef->GetType())
			{
			case RT_LineGeometry:
				PushDrawCall(handle, resMgr->GetLineGeometry(handle));
				break;
			default:
				break;
			}
		}
	}
}

void RenderableObject::SetBufferDirty()
{
	_isDirty = _isBufferDirty = true;
}
