//
// MPF Platform
// Direct3D 9 Vertex
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#pragma once
#include "../../inc/common.h"
#include <d3d9.h>
#include <DirectXMath.h>

DEFINE_NS_PLATFORM

namespace D3D
{
#pragma pack(push, 4)

	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

#pragma pack(pop)
#pragma pack(push, 16)

	struct ConstantBufferData
	{
		DirectX::XMFLOAT4X4 World;
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Projection;
	};

#pragma pack(pop)
}

END_NS_PLATFORM