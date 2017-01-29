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
	:_isDirty(true), _isBufferDirty(true), _parent(nullptr), _translation(0, 0, 0), _modelTransform(XMMatrixIdentity())
{

}

void RenderableObject::SetParent(RenderableObject* parent)
{
	_parent = parent;
	UpdateTransform();
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
	_translation = { x, y, 0 };
	UpdateTransform();
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
	{
		XMFLOAT4X4 model;
		XMStoreFloat4x4(&model, XMMatrixTranspose(_modelTransform));
		drawCallList->Draw(model);
	}
}

void RenderableObject::SetBufferDirty()
{
	_isDirty = _isBufferDirty = true;
}

void RenderableObject::UpdateTransform()
{
	const auto x = _translation.x;
	const auto y = _translation.y;

	if (_parent)
		_modelTransform = _parent->_modelTransform * XMMatrixTranslation(x, y, 0);
	else
		_modelTransform = XMMatrixIdentity() * XMMatrixTranslation(x, y, 0);
}
