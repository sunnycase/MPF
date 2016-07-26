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
	if (_isDirty)
	{
		if (_buffer)
		{
			auto resMgr = _buffer->GetResourceManager();
			auto drawCallList = resMgr->CreateDrawCallList();
			for (auto&& geoRef : _buffer->GetGeometries())
				drawCallList->PushDrawCall(geoRef.Get());
			_drawCallList = drawCallList;
		}
		else
			_drawCallList.reset();
		_isDirty = false;
	}
}

void RenderableObject::Draw()
{
	if (auto drawCallList = _drawCallList)
		drawCallList->Draw();
}

void RenderableObject::SetBufferDirty()
{
	_isDirty = _isBufferDirty = true;
}
