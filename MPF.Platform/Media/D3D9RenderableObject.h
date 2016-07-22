//
// MPF Platform
// Renderable Object
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "RenderableObject.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9RenderableObject : public RenderableObject
{
public:
	D3D9RenderableObject();

	void Update();
protected:
	virtual void PushDrawCall(UINT_PTR handle, const LineGeometry& geometry) override;
};

END_NS_PLATFORM