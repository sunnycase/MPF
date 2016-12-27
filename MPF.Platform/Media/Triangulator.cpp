//
// MPF Platform
// Triangulator
// 作者：SunnyCase
// 创建时间：2016-12-27
//
#include "stdafx.h"
#include "Triangulator.h"
#include <Triangle/triangle.h>
#include <algorithm>

using namespace WRL;
using namespace NS_PLATFORM;

namespace
{
	bool ContourCounterClockWise(const std::vector<std::pair<size_t, size_t>>& segments, const std::vector<Point>& points)
	{
		/// Calculating the polygon area. 
		/// A > 0 => polygon counter clockwise oriented
		int area = 0;

		for(auto&& segment : segments)
		{
			const Point& start = points[segment.first];
			const Point& end = points[segment.second];

			area += ((start.X * end.Y) - (end.X * start.Y));
		}

		area *= 0.5f;

		return area > 0;
	}
}

Triangulator::Triangulator(const std::vector<Point>& points, const std::vector<std::vector<std::pair<size_t, size_t>>>& contours)
{
	_points.reserve(points.size() * 2);
	std::for_each(points.begin(), points.end(), [&](const Point& pt) {
		_points.emplace_back(pt.X);
		_points.emplace_back(pt.Y);
	});

	_pointMarkers.resize(points.size());

	int contourId = 2;
	for (auto&& contour : contours)
	{
		for (auto&& segment : contour)
		{
			_segments.emplace_back(int(segment.first));
			_segments.emplace_back(int(segment.second));

			_pointMarkers[segment.first] = contourId;
			_pointMarkers[segment.second] = contourId;
			_segmentMarkers.emplace_back(contourId);
		}

		auto isHole = ContourCounterClockWise(contour, points);
		if (isHole)
		{
			Point middlePoint{};
			for (auto&& segment : contour)
			{
				middlePoint.X += points[segment.first].X;
				middlePoint.Y += points[segment.first].Y;
			}
			middlePoint.X /= contour.size();
			middlePoint.Y /= contour.size();
			_holes.emplace_back(middlePoint.X);
			_holes.emplace_back(middlePoint.Y);
		}
		contourId++;
	}
}

void Triangulator::Triangulate(std::function<void(std::array<DirectX::XMFLOAT2, 3>&)>&& outputTriangle)
{
	triangulateio in{}, out{};
	in.numberofpoints = _points.size() / 2;
	in.pointlist = _points.data();
	in.pointmarkerlist = _pointMarkers.data();

	in.numberofsegments = _segments.size() / 2;
	in.segmentlist = _segments.data();
	in.segmentmarkerlist = _segmentMarkers.data();

	in.numberofholes = _holes.size() / 2;
	in.holelist = _holes.data();

	triangulate("pzQ", &in, &out, nullptr);

	for (size_t i = 0; i < out.numberoftriangles; i++)
	{
		std::array<DirectX::XMFLOAT2, 3> triangle;
		for (size_t j = 0; j < 3; j++)
		{
			size_t pointIdx = out.trianglelist[i * 3 + j];
			triangle[j].x = out.pointlist[pointIdx * 2];
			triangle[j].y = out.pointlist[pointIdx * 2 + 1];
		}
		outputTriangle(triangle);
	}

	free(out.pointlist);
	free(out.pointmarkerlist);
	free(out.trianglelist);
	free(out.neighborlist);
	free(out.segmentlist);
	free(out.segmentmarkerlist);
}
