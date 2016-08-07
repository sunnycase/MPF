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
using namespace DirectX;

RenderableObject::RenderableObject()
	:_isDirty(true), _isBufferDirty(true)
{
	XMStoreFloat4x4(&_modelTransform, XMMatrixTranspose(XMMatrixIdentity()));
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

void RenderableObject::SetOffset(float x, float y)
{
	XMStoreFloat4x4(&_modelTransform, XMMatrixTranspose(XMMatrixIdentity() * XMMatrixTranslation(x, y, 0)));
}

void RenderableObject::Update()
{
	if (_isDirty)
	{
		if (_buffer)
		{
			auto resMgr = _buffer->GetResourceManager();
			_drawCallList = resMgr->CreateDrawCallList(_buffer.Get());
		}
		else
			_drawCallList.reset();
		_isDirty = false;
	}
}

void RenderableObject::Draw()
{
	if (auto drawCallList = _drawCallList)
		drawCallList->Draw(_modelTransform);
}

void RenderableObject::SetBufferDirty()
{
	_isDirty = _isBufferDirty = true;
}
