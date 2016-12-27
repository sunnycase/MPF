//
// MPF Platform
// Triangulator
// 作者：SunnyCase
// 创建时间：2016-12-27
//
#pragma once
#include "../../inc/common.h"
#include "../inc/WeakReferenceBase.h"
#include <DirectXMath.h>
#include <vector>
#include <array>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class Triangulator
{
public:
	Triangulator(const std::vector<Point>& points, const std::vector<std::vector<std::pair<size_t, size_t>>>& contours);

	void Triangulate(std::function<void(std::array<DirectX::XMFLOAT2, 3>&)>&& outputTriangle);
private:
	std::vector<float> _points;
	std::vector<int> _pointMarkers;
	std::vector<int> _segments, _segmentMarkers;
	std::vector<float> _holes;
};

END_NS_PLATFORM