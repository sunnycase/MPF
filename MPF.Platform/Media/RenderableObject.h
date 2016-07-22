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

class RenderableObject : public ResourceBase
{
public:
	RenderableObject();

	void SetContent(IRenderCommandBuffer* buffer);
	void Update();
protected:
	virtual void PushDrawCall(UINT_PTR handle, const LineGeometry& geometry) = 0;
private:
	void SetBufferDirty();
private:
	bool _isDirty;
	bool _isBufferDirty;
	WRL::ComPtr<RenderCommandBuffer> _buffer;
};

END_NS_PLATFORM