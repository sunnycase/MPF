//
// MPF Platform
// Direct3D 11 Geometry Transforms
// 作者：SunnyCase
// 创建时间：2016-12-25
//
#include "stdafx.h"
#include "../D3D11PlatformProvider.h"
#include "../../Triangulator.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;
using namespace NS_PLATFORM_D3D11;

namespace
{
	bool IsControlPointOutter(XMFLOAT2 startPoint, XMFLOAT2 endPoint, XMFLOAT2 control)
	{
		auto startEndVec = XMLoadFloat2(&endPoint) - XMLoadFloat2(&startPoint);
		auto startControlVec = XMLoadFloat2(&control) - XMLoadFloat2(&startPoint);

		auto cross = XMVector2Cross(startEndVec, startControlVec);
		return cross.m128_f32[2] < 0;
	}

	void EmplaceQudraticBezier(std::vector<D3D11::FillVertex>& vertices, std::vector<size_t>& indices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, XMFLOAT2 control, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		float swit = IsControlPointOutter(startPoint, endPoint, control) ? 1 : -1;

		const auto count = vertices.size();
		vertices.emplace_back(D3D11::FillVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			{ 0, 0, swit }, D3D11::FillVertex::ST_QuadraticBezier
		});
		vertices.emplace_back(D3D11::FillVertex
		{
			{ control.x, control.y, 0.f },
			{ 0.5f, 0, swit }, D3D11::FillVertex::ST_QuadraticBezier
		});
		vertices.emplace_back(D3D11::FillVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			{ 1, 1, swit }, D3D11::FillVertex::ST_QuadraticBezier
		});

		for (auto&& i : { 0, 1, 2 })
			indices.emplace_back(count + i);
	}

	void SwapIfGeater(float& a, float& b)
	{
		if (a > b)
			std::swap(a, b);
	}
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::StrokeVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry)
{
	XMFLOAT2 startPoint{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y };
	XMFLOAT2 endPoint{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y };

	XMFLOAT2 normal, normalOpp;
	const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(XMLoadFloat2(&endPoint) - XMLoadFloat2(&startPoint)));
	XMStoreFloat2(&normal, normalVec);
	XMStoreFloat2(&normalOpp, XMVectorScale(normalVec, -1.f));

	vertices.emplace_back(D3D11::StrokeVertex
	{
		{ startPoint.x, startPoint.y, 0.f },
		{ normal.x, normal.y, 0.f },
		normal, D3D11::StrokeVertex::ST_Linear
	});
	vertices.emplace_back(D3D11::StrokeVertex
	{
		{ endPoint.x, endPoint.y, 0.f },
		{ normal.x, normal.y, 0.f },
		normal, D3D11::StrokeVertex::ST_Linear
	});
	vertices.emplace_back(D3D11::StrokeVertex
	{
		{ endPoint.x, endPoint.y, 0.f },
		{ normalOpp.x, normalOpp.y, 0.f },
		normalOpp, D3D11::StrokeVertex::ST_Linear
	});
	vertices.emplace_back(D3D11::StrokeVertex
	{
		{ startPoint.x, startPoint.y, 0.f },
		{ normalOpp.x, normalOpp.y, 0.f },
		normalOpp, D3D11::StrokeVertex::ST_Linear
	});

	// triangle 1
	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);

	// triangle 2
	indices.emplace_back(2);
	indices.emplace_back(3);
	indices.emplace_back(0);
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::StrokeVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry)
{
	auto leftTopPoint = geometry.Data.LeftTop;
	auto rightBottomPoint = geometry.Data.RightBottom;
	SwapIfGeater(leftTopPoint.X, rightBottomPoint.X);
	SwapIfGeater(leftTopPoint.Y, rightBottomPoint.Y);

	XMFLOAT2 pt[4]{
		{ leftTopPoint.X, leftTopPoint.Y },
		{ rightBottomPoint.X, leftTopPoint.Y },
		{ rightBottomPoint.X, rightBottomPoint.Y },
		{ leftTopPoint.X, rightBottomPoint.Y }
	};

	const XMFLOAT2 normal[4] = {
		{ -1, -1 },
		{ 1, -1 },
		{ 1, 1 },
		{ -1, 1 }
	};

	for (size_t i = 0; i < 8; i++)
	{
		const auto pIdx = i % 4;
		const auto nIdx = i > 3 ? (i % 2 ? 8 - i : 6 - i) : i;
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ pt[pIdx].x, pt[pIdx].y, 0.f },
			{ normal[nIdx].x, normal[nIdx].y, 0.f },
			normal[nIdx], D3D11::StrokeVertex::ST_Linear
		});
	}

	for (auto&& i : {
		0, 1, 5, 5, 4, 0,
		1, 2, 6, 6, 5, 1,
		2, 3, 7, 7, 6, 2,
		3, 0, 4, 4, 7, 3
	})
		indices.emplace_back(i);
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::StrokeVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry)
{
	using namespace PathGeometrySegments;

	bool lastOpIsLineTo = false;
	XMFLOAT2 lastPoint{ 0,0 };
	size_t count = 0;
	for (auto&& seg : geometry.Segments)
	{
		switch (seg.Operation)
		{
		case MoveTo:
		{
			const auto& data = seg.Data.MoveTo;
			lastPoint = { data.Point.X, data.Point.Y };
			lastOpIsLineTo = false;
		}
		break;
		case LineTo:
		{
			const auto& data = seg.Data.LineTo;
			XMFLOAT2 startPoint = lastPoint;
			XMFLOAT2 endPoint{ data.Point.X, data.Point.Y };

			XMFLOAT2 normal, normalOpp;
			const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(XMLoadFloat2(&endPoint) - XMLoadFloat2(&startPoint)));
			XMStoreFloat2(&normal, normalVec);
			XMStoreFloat2(&normalOpp, XMVectorScale(normalVec, -1.f));

			size_t last3Idx, last2Idx;

			if (!lastOpIsLineTo)
			{
				vertices.emplace_back(D3D11::StrokeVertex
				{
					{ startPoint.x, startPoint.y, 0.f },
					{ normal.x, normal.y, 0.f },
					normal, D3D11::StrokeVertex::ST_Linear
				});
			}
			else
			{
				last3Idx = *(indices.end() - 5);
				auto& vertex = vertices[last3Idx];
				vertex.Normal = { normal.x, normal.y, 0.f };
				vertex.ParamFormValue.x += normal.x;
				vertex.ParamFormValue.y += normal.y;
			}
			vertices.emplace_back(D3D11::StrokeVertex
			{
				{ endPoint.x, endPoint.y, 0.f },
				{ normal.x, normal.y, 0.f },
				normal, D3D11::StrokeVertex::ST_Linear
			});
			vertices.emplace_back(D3D11::StrokeVertex
			{
				{ endPoint.x, endPoint.y, 0.f },
				{ normalOpp.x, normalOpp.y, 0.f },
				normalOpp, D3D11::StrokeVertex::ST_Linear
			});
			if (!lastOpIsLineTo)
			{
				vertices.emplace_back(D3D11::StrokeVertex
				{
					{ startPoint.x, startPoint.y, 0.f },
					{ normalOpp.x, normalOpp.y, 0.f },
					normalOpp, D3D11::StrokeVertex::ST_Linear
				});
			}
			else
			{
				last2Idx = *(indices.end() - 3);
				auto& vertex = vertices[last2Idx];
				vertex.Normal = { normalOpp.x, normalOpp.y, 0.f };
				vertex.ParamFormValue.x += normalOpp.x;
				vertex.ParamFormValue.y += normalOpp.y;
			}

			if (lastOpIsLineTo)
			{
				indices.emplace_back(last3Idx);
				indices.emplace_back(0 + count);
				indices.emplace_back(1 + count);
				indices.emplace_back(1 + count);
				indices.emplace_back(last2Idx);
				indices.emplace_back(last3Idx);
				count += 2;
			}
			else
			{
				for (auto&& i : {
					0, 1, 2,
					2, 3, 0 })
					indices.emplace_back(count + i);
				count += 4;
			}

			lastPoint = endPoint;
			lastOpIsLineTo = true;
		}
		break;
		case ArcTo:
		{
			const auto& data = seg.Data.ArcTo;
			XMFLOAT2 startPoint = lastPoint;
			XMFLOAT2 endPoint(data.Point.X, data.Point.Y);

			//const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
			//const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

			//EmplaceArc(vertices, lastPoint, endPoint, data.Angle, normalVec, normalVec);
			lastPoint = endPoint;
			lastOpIsLineTo = false;
		}
		break;
		case QuadraticBezierTo:
		{
			const auto& data = seg.Data.QuadraticBezierTo;
			XMFLOAT2 startPoint = lastPoint;
			XMFLOAT2 endPoint(data.Point.X, data.Point.Y);
			XMFLOAT2 control(data.Control.X, data.Control.Y);

			vertices.emplace_back(D3D11::StrokeVertex
			{
				{ startPoint.x, startPoint.y, 0.f },
				{ 0, 0, 0 },
				{ 0, 0 }, D3D11::StrokeVertex::ST_QuadraticBezier
			});
			vertices.emplace_back(D3D11::StrokeVertex
			{
				{ control.x, control.y, 0.f },
				{ 0, 0, 0 },
				{ 0.5f, 0 }, D3D11::StrokeVertex::ST_QuadraticBezier
			});
			vertices.emplace_back(D3D11::StrokeVertex
			{
				{ endPoint.x, endPoint.y, 0.f },
				{ 0, 0, 0 },
				{ 1, 1 }, D3D11::StrokeVertex::ST_QuadraticBezier
			});

			for (auto&& i : { 0, 1, 2 })
				indices.emplace_back(count + i);
			count += 3;

			lastPoint = endPoint;
			lastOpIsLineTo = false;
		}
		break;
		default:
			ThrowAlways(L"Invalid path segment.");
		}
	}
}


void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::FillVertex>& vertices, std::vector<size_t>& indices, const LineGeometry& geometry)
{

}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::FillVertex>& vertices, std::vector<size_t>& indices, const RectangleGeometry& geometry)
{
	auto leftTopPoint = geometry.Data.LeftTop;
	auto rightBottomPoint = geometry.Data.RightBottom;
	SwapIfGeater(leftTopPoint.X, rightBottomPoint.X);
	SwapIfGeater(leftTopPoint.Y, rightBottomPoint.Y);

	XMFLOAT2 leftTop{ leftTopPoint.X, leftTopPoint.Y };
	XMFLOAT2 rightTop{ rightBottomPoint.X, leftTopPoint.Y };
	XMFLOAT2 rightBottom{ rightBottomPoint.X, rightBottomPoint.Y };
	XMFLOAT2 leftBottom{ leftTopPoint.X, rightBottomPoint.Y };

	vertices.emplace_back(D3D11::FillVertex
	{
		{ leftTop.x, leftTop.y, 0.f }
	});
	vertices.emplace_back(D3D11::FillVertex
	{
		{ rightTop.x, rightTop.y, 0.f }
	});
	vertices.emplace_back(D3D11::FillVertex
	{
		{ rightBottom.x, rightBottom.y, 0.f }
	});
	vertices.emplace_back(D3D11::FillVertex
	{
		{ leftBottom.x, leftBottom.y, 0.f }
	});

	// triangle 1
	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);

	// triangle 2
	indices.emplace_back(2);
	indices.emplace_back(3);
	indices.emplace_back(0);
}

namespace
{
	bool operator==(const Point& left, const Point& right)
	{
		return left.X == right.X && left.Y == right.Y;
	}
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::FillVertex>& vertices, std::vector<size_t>& indices, const PathGeometry& geometry)
{
	using namespace PathGeometrySegments;
	{
		std::vector<Point> points;
		std::vector<std::vector<std::pair<size_t, size_t>>> contours;

		size_t cntSegmentsIdx = -1;
		int pointIdx = -1; size_t startPointIdx = -1;
		Point lastPoint;
		for (auto&& seg : geometry.Segments)
		{
			switch (seg.Operation)
			{
			case MoveTo:
			{
				const auto& data = seg.Data.MoveTo;
				lastPoint = data.Point;
				points.emplace_back(data.Point);
				pointIdx++;
				startPointIdx = pointIdx;

				cntSegmentsIdx = contours.size();
				contours.emplace_back();
			}
			break;
			case LineTo:
			{
				const auto& data = seg.Data.LineTo;
				lastPoint = data.Point;
				if (lastPoint == points[startPointIdx])
					contours[cntSegmentsIdx].emplace_back(pointIdx, startPointIdx);
				else
				{
					points.emplace_back(data.Point);
					contours[cntSegmentsIdx].emplace_back(pointIdx, pointIdx + 1);
					pointIdx++;
				}
			}
			break;
			case ArcTo:
			{
				const auto& data = seg.Data.ArcTo;
				lastPoint = data.Point;
				if (lastPoint == points[startPointIdx])
					contours[cntSegmentsIdx].emplace_back(pointIdx, startPointIdx);
				else
				{
					points.emplace_back(data.Point);
					contours[cntSegmentsIdx].emplace_back(pointIdx, pointIdx + 1);
					pointIdx++;
				}
			}
			break;
			case QuadraticBezierTo:
			{
				const auto& data = seg.Data.QuadraticBezierTo;
				XMFLOAT2 endPoint(data.Point.X, data.Point.Y);
				if (IsControlPointOutter({ lastPoint.X, lastPoint.Y }, endPoint, { data.Control.X, data.Control.Y }))
				{
					points.emplace_back(data.Control);
					contours[cntSegmentsIdx].emplace_back(pointIdx, pointIdx + 1);
					pointIdx++;

					lastPoint = data.Point;
					if (lastPoint == points[startPointIdx])
						contours[cntSegmentsIdx].emplace_back(pointIdx, startPointIdx);
					else
					{
						points.emplace_back(data.Point);
						contours[cntSegmentsIdx].emplace_back(pointIdx, pointIdx + 1);
						pointIdx++;
					}
				}
				else
				{
					lastPoint = data.Point;
					if (lastPoint == points[startPointIdx])
						contours[cntSegmentsIdx].emplace_back(pointIdx, startPointIdx);
					else
					{
						points.emplace_back(data.Point);
						contours[cntSegmentsIdx].emplace_back(pointIdx, pointIdx + 1);
						pointIdx++;
					}
				}
			}
			break;
			default:
				break;
			}
		}
		if (points.size() > 2 && std::all_of(contours.begin(), contours.end(), [](auto& segments) {return segments.front().first == segments.back().second; }))
		{
			Triangulator triangulator(points, contours);
			triangulator.Triangulate([&](XMFLOAT2 pt) {
				vertices.emplace_back(D3D11::FillVertex
				{
					{ pt.x, pt.y, 0.f }
				});
			},
				[&](const std::array<size_t, 3>& t) {
				for (auto&& i : t)
					indices.emplace_back(i);
			});
		}
		else
			return;
	}

	{
		XMFLOAT2 lastPoint{ 0,0 };
		for (auto&& seg : geometry.Segments)
		{
			switch (seg.Operation)
			{
			case MoveTo:
			{
				const auto& data = seg.Data.MoveTo;
				lastPoint = { data.Point.X, data.Point.Y };
			}
			break;
			case LineTo:
			{
				const auto& data = seg.Data.LineTo;
				XMFLOAT2 endPoint(data.Point.X, data.Point.Y);
				lastPoint = endPoint;
			}
			break;
			case ArcTo:
			{
				const auto& data = seg.Data.ArcTo;
				XMFLOAT2 endPoint(data.Point.X, data.Point.Y);

				const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
				const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

				//EmplaceArc(vertices, lastPoint, endPoint, data.Angle, normalVec, normalVec);
				lastPoint = endPoint;
			}
			break;
			case QuadraticBezierTo:
			{
				const auto& data = seg.Data.QuadraticBezierTo;
				XMFLOAT2 endPoint(data.Point.X, data.Point.Y);

				const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
				const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

				EmplaceQudraticBezier(vertices, indices, lastPoint, endPoint, { data.Control.X, data.Control.Y }, normalVec, normalVec);
				lastPoint = endPoint;
			}
			break;
			default:
				break;
			}
		}
	}
}

// 3D Geometry

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::Fill3DVertex>& vertices, std::vector<size_t>& indices, const BoxGeometry3D& geometry)
{
	const auto position = geometry.Data.Position;
	const auto width = geometry.Data.Width;
	const auto height = geometry.Data.Height;
	const auto depth = geometry.Data.Depth;

	XMFLOAT3 pt1{ position.X, position.Y, position.Z };
	XMFLOAT3 pt2{ position.X + width, position.Y, position.Z };
	XMFLOAT3 pt3{ position.X + width, position.Y + height, position.Z };
	XMFLOAT3 pt4{ position.X, position.Y + height, position.Z };
	XMFLOAT3 pt5{ position.X, position.Y, position.Z + depth };
	XMFLOAT3 pt6{ position.X + width, position.Y, position.Z + depth };
	XMFLOAT3 pt7{ position.X + width, position.Y + height, position.Z + depth };
	XMFLOAT3 pt8{ position.X, position.Y + height, position.Z + depth };

	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt1.x, pt1.y, pt1.z },
		//{ 0, 0, 0}, FillVertex::ST_Linear,
		//{ 0, 0 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt2.x, pt2.y, pt2.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 0 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt3.x, pt3.y, pt3.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 1 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt4.x, pt4.y, pt4.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 0 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt5.x, pt5.y, pt5.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 0, 0 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt6.x, pt6.y, pt6.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 0 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt7.x, pt7.y, pt7.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 1 }
	});
	vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ pt8.x, pt8.y, pt8.z },
		//{ 0, 0, 0 }, FillVertex::ST_Linear,
		//{ 1, 0 }
	});

	// face 1
	indices.emplace_back(0); indices.emplace_back(1); indices.emplace_back(2);
	indices.emplace_back(0); indices.emplace_back(2); indices.emplace_back(3);

	// face 2
	indices.emplace_back(4); indices.emplace_back(0); indices.emplace_back(3);
	indices.emplace_back(3); indices.emplace_back(7); indices.emplace_back(4);

	// face 3
	indices.emplace_back(4); indices.emplace_back(5); indices.emplace_back(1);
	indices.emplace_back(1); indices.emplace_back(0); indices.emplace_back(4);

	// face 4
	indices.emplace_back(5); indices.emplace_back(4); indices.emplace_back(7);
	indices.emplace_back(7); indices.emplace_back(6); indices.emplace_back(5);

	// face 5
	indices.emplace_back(1); indices.emplace_back(5); indices.emplace_back(6);
	indices.emplace_back(6); indices.emplace_back(2); indices.emplace_back(1);

	// face 6
	indices.emplace_back(2); indices.emplace_back(6); indices.emplace_back(7);
	indices.emplace_back(7); indices.emplace_back(3); indices.emplace_back(2);
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<D3D11::Fill3DVertex>& vertices, std::vector<size_t>& indices, const MeshGeometry3D& geometry)
{
	for (auto&& vertex : geometry.Vertices)
		vertices.emplace_back(D3D11::Fill3DVertex
	{
		{ vertex.Position.X, vertex.Position.Y, vertex.Position.Z },
		//{ vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z },
		//FillVertex::ST_Linear,
		//{ vertex.TexCoord.X, vertex.TexCoord.Y }
	});

	for (auto&& index : geometry.Indices)
		indices.emplace_back(index);
}