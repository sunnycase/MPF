//
// MPF Platform
// Resource Transform Primitives
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "../../inc/common.h"
#include <vector>
#include "../Geometry.h"

DEFINE_NS_PLATFORM

struct RentInfo
{
	size_t entryIdx;
	size_t offset;
	size_t length;
};

template<class TRenderCall>
struct StorkeRenderCall : public TRenderCall
{
	float Thickness;
	float Color[4];
};

template<class TRenderCall>
struct FillRenderCall : public TRenderCall
{
	float Color[4];
};

enum class PlatformId
{
	D3D9,
	D3D11,
	OpenGL
};

template<PlatformId>
struct PlatformProvider
{
	using RenderCall = void;
	using VertexBufferManager = void;
	using StrokeVertex = void;
	using FillVertex = void;

	void Transform(std::vector<StrokeVertex>& vertices, const LineGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, const RectangleGeometry& geometry) {}
	void Transform(std::vector<StrokeVertex>& vertices, const PathGeometry& geometry) {}
};

END_NS_PLATFORM