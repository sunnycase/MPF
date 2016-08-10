//
// MPF Platform
// Geometry
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include <DirectXMath.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct LineGeometry : public ResourceBase
{
	LineGeometryData Data;
};

struct RectangleGeometry : public ResourceBase
{
	RectangleGeometryData Data;
};

namespace PathGeometrySegments
{
	enum Operations : uint32_t
	{
		MoveTo,
		LineTo,
		ArcTo,
		QuadraticBezierTo,
		CubicBezierTo
	};

	struct Segment
	{
		Operations Operation;
		union tagData
		{
			struct tagMoveTo
			{
				Point Point;
			} MoveTo;

			struct tagLineTo
			{
				Point Point;
			} LineTo;

			struct tagArcTo
			{
				Point Point;
				float Angle;
			} ArcTo;

			struct tagQuadraticBezierTo
			{
				Point Control;
				Point Point;
			} QuadraticBezierTo;

			struct tagCubicBezierTo
			{
				Point Control1;
				Point Control2;
				Point Point;
			} CubicBezierTo;
		} Data;
	};
}

struct PathGeometry : public ResourceBase
{
private:
	using Segment = PathGeometrySegments::Segment;
public:
	std::vector<Segment> Segments;
};

END_NS_PLATFORM