//
// MPF Platform
// Direct3D 9 Vertex
// 作者：SunnyCase
// 创建时间：2016-07-25
//
#pragma once
#include "../../inc/common.h"
#include <DirectXMath.h>

DEFINE_NS_PLATFORM

namespace D3D11
{
#pragma pack(push, 4)

	struct StrokeVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Data1;		// ST_Linear: Normal, ST_QuadraticBezier: 
		DirectX::XMFLOAT2 Data2;
		float SegmentType;
		DirectX::XMFLOAT2 Data3;
		DirectX::XMFLOAT2 Data4;
		DirectX::XMFLOAT2 Data5;

		static constexpr float ST_Linear = 0;
		static constexpr float ST_QuadraticBezier = 1;
		static constexpr float ST_CubicBezier = 2;
		static constexpr float ST_Arc = 3;
	};

#pragma pack(pop)
#pragma pack(push, 16)

	struct WorldViewProjectionModelData
	{
		DirectX::XMFLOAT4X4 WorldView;
		DirectX::XMFLOAT4X4 Projection;
		DirectX::XMFLOAT4X4 Model;
	};

#pragma pack(pop)
}

END_NS_PLATFORM