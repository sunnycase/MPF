//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#include "stdafx.h"
#include "D3D9ResourceManager.h"
#include "ResourceRef.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;

#define CTOR_IMPL1(T) _trc##T##(_strokeVBMgr)

D3D9ResourceManager::D3D9ResourceManager(IDirect3DDevice9* device)
	:_device(device), _strokeVBMgr(device, sizeof(D3D::StrokeVertex)), CTOR_IMPL1(LineGeometry), CTOR_IMPL1(RectangleGeometry)
{
}

namespace
{
	void EmplaceLine(std::vector<D3D::StrokeVertex>& vertices, XMFLOAT2 startPoint, XMFLOAT2 endPoint, const XMVECTOR& normalStartVec, const XMVECTOR& normalEndVec)
	{
		XMFLOAT2 normalStart, normalStartOpp;
		XMFLOAT2 normalEnd, normalEndOpp;
		XMStoreFloat2(&normalStart, normalStartVec);
		XMStoreFloat2(&normalStartOpp, XMVectorScale(normalStartVec, -1.f));
		XMStoreFloat2(&normalEnd, normalEndVec);
		XMStoreFloat2(&normalEndOpp, XMVectorScale(normalEndVec, -1.f));

		const XMFLOAT4 paramCoffZero{ 0, 0, 0, 0 };

		// 1
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f},
			normalStart,{ 0, 0 }, paramCoffZero
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEnd,{ 1.f, 1.f }, paramCoffZero
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, paramCoffZero
		});

		// 2
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ endPoint.x, endPoint.y, 0.f },
			normalEndOpp,{ 1.f, 1.f }, paramCoffZero
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStartOpp,{ 0, 0 }, paramCoffZero
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ startPoint.x, startPoint.y, 0.f },
			normalStart,{ 0, 0 }, paramCoffZero
		});
	}

	void SwapIfGeater(float& a, float& b)
	{
		if (a > b)
			std::swap(a, b);
	}
}

void ::NS_PLATFORM::Transform(std::vector<D3D::StrokeVertex>& vertices, const LineGeometry& geometry)
{
	const auto dirVec = XMLoadFloat2(&XMFLOAT2{ geometry.Data.EndPoint.X - geometry.Data.StartPoint.X, geometry.Data.EndPoint.Y - geometry.Data.StartPoint.Y });
	const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));

	EmplaceLine(vertices, { geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y }, { geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y },
		normalVec, normalVec);
}

void ::NS_PLATFORM::Transform(std::vector<D3D::StrokeVertex>& vertices, const RectangleGeometry& geometry)
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

	const XMFLOAT4 paramCoff{ 0, 1, 0, -1 };
	vertices.emplace_back(D3D::StrokeVertex
	{
		{ 0, 0, 0.f },
		{ 0 ,1 },{ 0, 0 }, paramCoff
	});
	vertices.emplace_back(D3D::StrokeVertex
	{
		{ 100, 0, 0.f },
		{ 0 ,1 },{ 0.5f, 0 }, paramCoff
	});
	vertices.emplace_back(D3D::StrokeVertex
	{
		{ 200, 200, 0.f },
		{ 3.f ,1 },{ 1, 1 }, paramCoff
	});
}

void D3D9ResourceManager::UpdateOverride()
{
	_strokeVBMgr.Upload();
}

namespace
{
	class DrawCallList : public IDrawCallList
	{
	public:
		DrawCallList(IDirect3DDevice9* device, D3D9ResourceManager* resMgr, RenderCommandBuffer* rcb)
			:_device(device), _resMgr(resMgr), _rcb(rcb)
		{

		}

		// 通过 IDrawCallList 继承
		virtual void Draw() override
		{
			float constants[4] = { 0 };
			for (auto&& rc : _strokeRenderCalls)
			{
				ThrowIfFailed(_device->SetStreamSource(0, rc.VB.Get(), 0, rc.Stride));
				constants[0] = rc.Thickness;
				ThrowIfFailed(_device->SetVertexShaderConstantF(12, constants, 1));
				ThrowIfFailed(_device->SetVertexShaderConstantF(16, rc.Color, 1));
				ThrowIfFailed(_device->DrawPrimitive(D3DPT_TRIANGLELIST, rc.StartVertex, rc.PrimitiveCount));
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
		void PushGeometryDrawCall(IResource* resource, IResource* pen)
		{
			if (pen)
			{
				StorkeRenderCall rc;
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
						_strokeRenderCalls.emplace_back(rc);
				}
			}
		}

		void Update(bool addResDependent)
		{
			_strokeRenderCalls.clear();
			for (auto&& geoRef : _rcb->GetGeometries())
			{
				PushGeometryDrawCall(geoRef.Geometry.Get(), geoRef.Pen.Get());
				if (addResDependent)
				{
					auto me = shared_from_this();
					_resMgr->AddDependentDrawCallList(me, geoRef.Geometry.Get());
					_resMgr->AddDependentDrawCallList(me, geoRef.Pen.Get());
				}
			}
		}
	private:
		ComPtr<IDirect3DDevice9> _device;
		ComPtr<D3D9ResourceManager> _resMgr;
		ComPtr<RenderCommandBuffer> _rcb;
		std::vector<StorkeRenderCall> _strokeRenderCalls;
	};
}

std::shared_ptr<IDrawCallList> D3D9ResourceManager::CreateDrawCallList(RenderCommandBuffer* rcb)
{
	auto ret = std::make_shared<DrawCallList>(_device.Get(), this, rcb);
	ret->Initialize();
	return ret;
}

#define TRYGET_IMPL1(T)										 \
	case RT_##T:											 \
		return _trc##T.TryGet(resRef->GetHandle(), rc);

bool D3D9ResourceManager::TryGet(IResource* res, StorkeRenderCall& rc) const
{
	auto resRef = static_cast<ResourceRef*>(res);
	switch (resRef->GetType())
	{
		TRYGET_IMPL1(LineGeometry);
		TRYGET_IMPL1(RectangleGeometry);
	default:
		break;
	}
	return false;
}