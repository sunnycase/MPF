//
// MPF Platform
// Material
// 作者：SunnyCase
// 创建时间：2017-02-14
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "ResourceRef.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct ShaderParameters : public ResourceBase
{
	std::vector<WRL::ComPtr<ResourceRef>> Brushes;
	std::vector<byte> Data;
};

struct Material : public ResourceBase
{
	WRL::ComPtr<IShadersGroup> Shader;
	WRL::ComPtr<ResourceRef> ShaderParameters;
};

END_NS_PLATFORM