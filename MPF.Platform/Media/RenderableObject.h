//
// MPF Platform
// Renderable Object
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "Geometry.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

DEFINE_ENUM_FLAG_OPERATORS(RenderableObjectFlags);

struct IDrawCallList;
typedef void(_stdcall *RenderableObjectAction)();

class RenderableObject : public ResourceBase
{
public:
	RenderableObject();

	void SetFlags(RenderableObjectFlags flags) { _flags |= flags; }
	void SetMeasureCallback(RenderableObjectAction callback) { _measureCallback = callback; }
	void SetContent(IRenderCommandBuffer* buffer);
	void Update();
	void Draw();
private:
	void SetBufferDirty();
private:
	bool _isDirty;
	bool _isBufferDirty;
	WRL::ComPtr<RenderCommandBuffer> _buffer;
	std::shared_ptr<IDrawCallList> _drawCallList;
	RenderableObjectFlags _flags;
	RenderableObjectAction _measureCallback = nullptr;
};

END_NS_PLATFORM