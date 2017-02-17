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

struct ShadersGroup
{
	std::vector<byte> VertexShader;
	std::vector<byte> PixelShader;
};

struct ShaderParameters
{
	std::vector<WRL::ComPtr<ResourceRef>> Brushes;
	std::vector<byte> Variables;
};

struct Material
{
	WRL::ComPtr<IResource> Shader;
	WRL::ComPtr<IResource> ShaderParameters;
};

END_NS_PLATFORM