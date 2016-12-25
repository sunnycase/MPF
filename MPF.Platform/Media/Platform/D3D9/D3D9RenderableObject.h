//
// MPF Platform
// Renderable Object
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "../../RenderableObject.h"
#include "../D3D9PlatformProvider.h"

DEFINE_NS_PLATFORM_D3D9

class D3D9RenderableObject : public RenderableObject
{
public:
	D3D9RenderableObject();

	void Update();
protected:
};

END_NS_PLATFORM_D3D9