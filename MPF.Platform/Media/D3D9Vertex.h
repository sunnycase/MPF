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

	struct StrokeVertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT2 Normal;
		DirectX::XMFLOAT2 ParamFormValue;
		float SegmentType;

		static constexpr float ST_Linear = 0;
		static constexpr float ST_QuadraticBezier = 1;
		static constexpr float ST_CubicBezier = 2;
		static constexpr float ST_Arc = 3;
	};

#pragma pack(pop)
#pragma pack(push, 16)

	struct WorldViewProjectionData
	{
		DirectX::XMFLOAT4X4 WorldView;
		DirectX::XMFLOAT4X4 Projection;
	};

	enum VertexShaderConstantSlots
	{
		VSCSlot_WorldViewProjection = 0,
		VSCSize_WorldViewProjection = sizeof(WorldViewProjectionData) / 16,
		VSCSlot_Thickness = VSCSlot_WorldViewProjection + VSCSize_WorldViewProjection,
		VSCSize_Thickness = 1,
		VSCSlot_Color = VSCSlot_Thickness + VSCSize_Thickness,
		VSCSize_Color = 1,
		VSCSlot_ModelTransform = VSCSlot_Color + VSCSize_Color,
		VSCSize_ModelTransform = 4,
		VSCSlot_GeometryTransform = VSCSlot_ModelTransform + VSCSize_ModelTransform,
		VSCSize_GeometryTransform = 4
	};

#pragma pack(pop)
}

END_NS_PLATFORM