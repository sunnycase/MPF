//
// MPF Platform
// Direct3D 9 Geometry Transforms
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#include "stdafx.h"
#include "../D3D9PlatformProvider.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;
using namespace NS_PLATFORM_D3D9;

namespace
{
	void EmplaceLine(std::vector<StrokeVertex>& vertices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		// 1
		vertices.emplace_back(StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStart,{ 0, 0 }, StrokeVertex::ST_Linear
		});
		vertices.emplace_back(StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEnd,{ 1.f, 1.f }, StrokeVertex::ST_Linear
		});
		vertices.emplace_back(StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, StrokeVertex::ST_Linear
		});

		// 2
		vertices.emplace_back(StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, StrokeVertex::ST_Linear
		});
		vertices.emplace_back(StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStartOpp,{ 0, 0 }, StrokeVertex::ST_Linear
		});
		vertices.emplace_back(StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStart,{ 0, 0 }, StrokeVertex::ST_Linear
		});
	}

	void EmplaceArc(std::vector<StrokeVertex>& vertices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, float angle, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - startPoint.x, endPoint.y - startPoint.y });
		const auto halfVec = XMVectorScale(dirVec, 0.5f);
		const auto centerDir = XMVector2Normalize(XMVector3Orthogonal(dirVec));
		const auto radian = XMConvertToRadians(angle);
		const auto radius = XMVector2Length(halfVec).m128_f32[0] / std::sin(radian / 2.f);
		const auto centerVec = XMLoadFloat2(&startPoint) + halfVec + centerDir * (radius * std::cos(radian / 2.f));
		const auto slopeLength = radius / std::cos(radian / 2.f) * 1.1f;
		const auto point2Vec = XMVector2Normalize(XMLoadFloat2(&startPoint) - centerVec) * slopeLength + centerVec;
		const auto point3Vec = XMVector2Normalize(XMLoadFloat2(&endPoint) - centerVec) * slopeLength + centerVec;
		XMFLOAT2 centerPoint, point2, point3;
		XMStoreFloat2(&centerPoint, centerVec);
		XMStoreFloat2(&point2, point2Vec);
		XMStoreFloat2(&point3, point3Vec);

		vertices.emplace_back(StrokeVertex
		{
			{ centerPoint.x, centerPoint.y, 0.f },
			{ 0 ,1 },{ 0, 0 }, StrokeVertex::ST_Arc
		});
		vertices.emplace_back(StrokeVertex
		{
			{ point2.x, point2.y, 0.f },
			{ 0 ,1 },{ slopeLength / radius, 0 }, StrokeVertex::ST_Arc
		});
		vertices.emplace_back(StrokeVertex
		{
			{ point3.x, point3.y, 0.f },
			{ 3.f ,1 },{ slopeLength * std::cos(radian) / radius, slopeLength * std::sin(radian) / radius }, StrokeVertex::ST_Arc
		});
	}

	void EmplaceQudraticBezier(std::vector<StrokeVertex>& vertices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, XMFLOAT2 control, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		XMVECTOR aVec = XMLoadFloat2(&startPoint);
		XMVECTOR bVec = XMLoadFloat2(&endPoint);
		XMVECTOR cVec = XMLoadFloat2(&control);
		auto mVec = (aVec + bVec + cVec) / 3.f;
		auto minLen = std::min(XMVector2Length(aVec - mVec).m128_f32[0], XMVector2Length(bVec - mVec).m128_f32[0]);
		minLen = std::min(minLen, XMVector2Length(cVec - mVec).m128_f32[0]);

		XMFLOAT2 mPoint;
		XMStoreFloat2(&mPoint, mVec);

		vertices.emplace_back(StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			mPoint,{ 0, 0 }, StrokeVertex::ST_QuadraticBezier,{ minLen, 0 }
		});
		vertices.emplace_back(StrokeVertex
		{
			{ control.x, control.y, 0.f },
			mPoint,{ 0.5f, 0 }, StrokeVertex::ST_QuadraticBezier,{ minLen, 0 }
		});
		vertices.emplace_back(StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			mPoint,{ 1, 1 }, StrokeVertex::ST_QuadraticBezier,{ minLen, 0 }
		});
	}

	void SwapIfGeater(float& a, float& b)
	{
		if (a > b)
			std::swap(a, b);
	}
}

void PlatformProvider<PlatformId::D3D9>::Transform(std::vector<StrokeVertex>& vertices, const LineGeometry& geometry)
{
	const auto dirVec = XMLoadFloat2(&XMFLOAT2{ geometry.Data.EndPoint.X - geometry.Data.StartPoint.X, geometry.Data.EndPoint.Y - geometry.Data.StartPoint.Y });
	const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

	EmplaceLine(vertices, { geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y }, { geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y },
		normalVec, normalVec);
}

void PlatformProvider<PlatformId::D3D9>::Transform(std::vector<StrokeVertex>& vertices, const RectangleGeometry& geometry)
{
	auto leftTopPoint = geometry.Data.LeftTop;
	auto rightBottomPoint = geometry.Data.RightBottom;
	SwapIfGeater(leftTopPoint.X, rightBottomPoint.X);
	SwapIfGeater(leftTopPoint.Y, rightBottomPoint.Y);

	XMFLOAT2 leftTop{ leftTopPoint.X, leftTopPoint.Y };
	XMFLOAT2 rightTop{ rightBottomPoint.X, leftTopPoint.Y };
	XMFLOAT2 rightBottom{ rightBottomPoint.X, rightBottomPoint.Y };
	XMFLOAT2 leftBottom{ leftTopPoint.X, rightBottomPoint.Y };

	const auto ltDirVec = XMLoadFloat2(&XMFLOAT2{ -1.f, -1.f });
	const auto rtDirVec = XMLoadFloat2(&XMFLOAT2{ 1.f, -1.f });
	const auto lbDirVec = XMLoadFloat2(&XMFLOAT2{ -1.f, 1.f });
	const auto rbDirVec = XMLoadFloat2(&XMFLOAT2{ 1.f, 1.f });

	EmplaceLine(vertices, leftTop, rightTop, ltDirVec, rtDirVec);
	EmplaceLine(vertices, rightTop, rightBottom, rtDirVec, rbDirVec);
	EmplaceLine(vertices, rightBottom, leftBottom, rbDirVec, lbDirVec);
	EmplaceLine(vertices, leftBottom, leftTop, lbDirVec, ltDirVec);
}

void PlatformProvider<PlatformId::D3D9>::Transform(std::vector<StrokeVertex>& vertices, const PathGeometry& geometry)
{
	using namespace PathGeometrySegments;

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

			const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
			const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

			EmplaceLine(vertices, lastPoint, endPoint, normalVec, normalVec);
			lastPoint = endPoint;
		}
		break;
		case ArcTo:
		{
			const auto& data = seg.Data.ArcTo;
			XMFLOAT2 endPoint(data.Point.X, data.Point.Y);

			const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
			const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

			EmplaceArc(vertices, lastPoint, endPoint, data.Angle, normalVec, normalVec);
			lastPoint = endPoint;
		}
		break;
		case QuadraticBezierTo:
		{
			const auto& data = seg.Data.QuadraticBezierTo;
			XMFLOAT2 endPoint(data.Point.X, data.Point.Y);

			const auto dirVec = XMLoadFloat2(&XMFLOAT2{ endPoint.x - lastPoint.x, endPoint.y - lastPoint.y });
			const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

			EmplaceQudraticBezier(vertices, lastPoint, endPoint, { data.Control.X, data.Control.Y }, normalVec, normalVec);
			lastPoint = endPoint;
		}
		break;
		default:
			break;
		}
	}
}