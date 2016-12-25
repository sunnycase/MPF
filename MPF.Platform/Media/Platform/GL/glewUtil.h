//
// MPF Platform
// OpenGL Glew Utils
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#pragma once
#include "../../inc/common.h"
#include <GL/glew.h>
#include "../inc/encoding.h"
DEFINE_NS_PLATFORM

void ThrowGlewIfNotOK(GLenum value)
{
	if (value != GLEW_OK)
	{
		auto msg = s2ws((char*)glewGetErrorString(value));
		ThrowAlways(msg.c_str());
	}
}

END_NS_PLATFORM