//
// MPF Platform
// D3D11 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#include "stdafx.h"
#include "D3D11ResourceManager.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;

namespace
{
	void EmplaceLine(std::vector<D3D11::StrokeVertex>& vertices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		// 1
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStart,{ 0, 0 }, D3D11::StrokeVertex::ST_Linear
		});
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEnd,{ 1.f, 1.f }, D3D11::StrokeVertex::ST_Linear
		});
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, D3D11::StrokeVertex::ST_Linear
		});

		// 2
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, D3D11::StrokeVertex::ST_Linear
		});
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStartOpp,{ 0, 0 }, D3D11::StrokeVertex::ST_Linear
		});
		vertices.emplace_back(D3D11::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStart,{ 0, 0 }, D3D11::StrokeVertex::ST_Linear
		});
	}
}

void ::NS_PLATFORM::Transform(std::vector<D3D11::StrokeVertex>& vertices, const LineGeometry& geometry)
{
	const auto dirVec = XMLoadFloat2(&XMFLOAT2{ geometry.Data.EndPoint.X - geometry.Data.StartPoint.X, geometry.Data.EndPoint.Y - geometry.Data.StartPoint.Y });
	const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

	EmplaceLine(vertices, { geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y }, { geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y },
		normalVec, normalVec);
}

void ::NS_PLATFORM::Transform(std::vector<D3D11::StrokeVertex>& vertices, const RectangleGeometry& geometry)
{

}

void ::NS_PLATFORM::Transform(std::vector<D3D11::StrokeVertex>& vertices, const PathGeometry& geometry)
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

			//EmplaceQudraticBezier(vertices, lastPoint, endPoint, { data.Control.X, data.Control.Y }, normalVec, normalVec);
			lastPoint = endPoint;
		}
		break;
		default:
			break;
		}
	}
}

namespace
{
	class DrawCallList : public IDrawCallList
	{
		struct MyStrokeDrawCall : public StorkeRenderCall
		{
			DirectX::XMFLOAT4X4 Transform;
		};
	public:
		DrawCallList(ID3D11DeviceContext* deviceContext, SwapChainUpdateContext& updateContext, D3D11ResourceManager* resMgr, RenderCommandBuffer* rcb)
			:_deviceContext(deviceContext), _resMgr(resMgr), _rcb(rcb), _updateContext(updateContext)
		{

		}

		// 通过 IDrawCallList 继承
		virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) override
		{
			using namespace D3D11;

			{
				auto model = _updateContext.Model.Map(_deviceContext.Get());
				model->Model = modelTransform;
			}
			float constants[4] = { 0 };
			UINT offset = 0;
			for (auto&& rc : _strokeRenderCalls)
			{
				auto vb = _resMgr->GetVertexBuffer(rc);
				
				_deviceContext->IASetVertexBuffers(0, 1, &vb, &rc.Stride, &offset);
				constants[0] = rc.Thickness;
				_deviceContext->Draw(rc.VertexCount, rc.StartVertex);
			}
		}

		virtual void Update() override
		{
			Update(false);
		}

		void Initialize()
		{
			Update(true);
		}
	private:
		// 通过 IDrawCallList 继承
		void PushGeometryDrawCall(IResource* resource, IResource* pen, const DirectX::XMFLOAT4X4 transform)
		{
			if (pen)
			{
				MyStrokeDrawCall rc;
				auto& penObj = _resMgr->GetPen(static_cast<ResourceRef*>(pen)->GetHandle());
				rc.Thickness = penObj.Thickness;
				if (penObj.Brush)
				{
					auto& brushObj = _resMgr->GetBrush(penObj.Brush->GetHandle());
					if (typeid(brushObj) == typeid(Brush&))
					{
						auto& color = static_cast<SolidColorBrush&>(brushObj).Color;
						rc.Color[0] = color.R;
						rc.Color[1] = color.G;
						rc.Color[2] = color.B;
						rc.Color[3] = color.A;
					}
					if (_resMgr->TryGet(resource, rc))
					{
						rc.Transform = transform;
						_strokeRenderCalls.emplace_back(rc);
					}
					else
					{
						assert(false && "Geometry not found.");
					}
				}
			}
		}

		void Update(bool addResDependent)
		{
			_strokeRenderCalls.clear();
			for (auto&& geoRef : _rcb->GetGeometries())
			{
				PushGeometryDrawCall(geoRef.Geometry.Get(), geoRef.Pen.Get(), geoRef.Transform);
				if (addResDependent)
				{
					auto me = shared_from_this();
					_resMgr->AddDependentDrawCallList(me, geoRef.Geometry.Get());
					_resMgr->AddDependentDrawCallList(me, geoRef.Pen.Get());
				}
			}
		}
	private:
		ComPtr<ID3D11DeviceContext> _deviceContext;
		SwapChainUpdateContext& _updateContext;
		ComPtr<D3D11ResourceManager> _resMgr;
		ComPtr<RenderCommandBuffer> _rcb;
		std::vector<MyStrokeDrawCall> _strokeRenderCalls;
	};
}

#define CTOR_IMPL1(T) _trc##T##(_strokeVBMgr)

D3D11ResourceManager::D3D11ResourceManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext, SwapChainUpdateContext& updateContext)
	:_device(device), _deviceContext(deviceContext), _updateContext(updateContext), _strokeVBMgr(device, sizeof(D3D11::StrokeVertex)), CTOR_IMPL1(LineGeometry), CTOR_IMPL1(RectangleGeometry), CTOR_IMPL1(PathGeometry)
{
}

void D3D11ResourceManager::UpdateOverride()
{
	_strokeVBMgr.Upload(_deviceContext.Get());
}

std::shared_ptr<IDrawCallList> D3D11ResourceManager::CreateDrawCallList(RenderCommandBuffer* rcb)
{
	auto ret = std::make_shared<DrawCallList>(_deviceContext.Get(), _updateContext, this, rcb);
	ret->Initialize();
	return ret;
}

#define TRYGET_IMPL1(T)										 \
	case RT_##T:											 \
		return _trc##T.TryGet(resRef->GetHandle(), rc);

bool D3D11ResourceManager::TryGet(IResource* res, StorkeRenderCall& rc) const
{
	auto resRef = static_cast<ResourceRef*>(res);
	switch (resRef->GetType())
	{
		TRYGET_IMPL1(LineGeometry);
		TRYGET_IMPL1(RectangleGeometry);
		TRYGET_IMPL1(PathGeometry);
	default:
		break;
	}
	return false;
}