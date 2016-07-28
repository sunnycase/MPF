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

D3D9ResourceManager::D3D9ResourceManager(IDirect3DDevice9* device)
	:_device(device), _strokeVBMgr(device, sizeof(D3D::StrokeVertex)), _lineGeometryTRC(_strokeVBMgr)
{
}

D3D9LineGeometryTRC::D3D9LineGeometryTRC(D3D9VertexBufferManager & strokeVBMgr)
	: _strokeVBMgr(strokeVBMgr)
{
}

namespace
{
	void Transform(std::vector<D3D::StrokeVertex>& vertices, const LineGeometry& geometry)
	{
		const auto dirVec = XMLoadFloat2(&XMFLOAT2{ geometry.Data.EndPoint.X - geometry.Data.StartPoint.X, geometry.Data.EndPoint.Y - geometry.Data.StartPoint.Y });
		const auto normalVec = XMVector2Normalize(XMVector2Orthogonal(dirVec));
		XMFLOAT2 normal, normalOpp;
		XMStoreFloat2(&normal, normalVec);
		XMStoreFloat2(&normalOpp, XMVectorScale(normalVec, -1.f));

		const XMFLOAT4 color{ 0.f, 0.f, 0.f, 1.f };

		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y, 0.f },
			normal
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y, 0.f },
			normal
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y, 0.f },
			normalOpp
		});

		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y, 0.f },
			normalOpp
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y, 0.f },
			normalOpp
		});
		vertices.emplace_back(D3D::StrokeVertex
		{
			{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y, 0.f },
			normal
		});
	}
}

void D3D9LineGeometryTRC::Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	static std::vector<D3D::StrokeVertex> vertices;
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);
		vertices.clear();
		Transform(vertices, source);
		auto rent = _strokeVBMgr.Allocate(vertices.size());
		_strokeRentInfos.emplace(handle, rent);
		_strokeVBMgr.Update(rent, 0, vertices.data(), vertices.size());
	}
}

void D3D9LineGeometryTRC::Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	static std::vector<D3D::StrokeVertex> vertices;
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);
		vertices.clear();
		Transform(vertices, source);

		auto& oldRent = _strokeRentInfos.find(handle)->second;
		if (oldRent.length != vertices.size())
		{
			_strokeVBMgr.Retire(oldRent);
			oldRent = _strokeVBMgr.Allocate(vertices.size());
		}
		_strokeVBMgr.Update(oldRent, 0, vertices.data(), vertices.size());
	}
}

void D3D9LineGeometryTRC::Remove(const std::vector<UINT_PTR>& handles)
{
	for (auto&& handle : handles)
	{
		auto oldRent = _strokeRentInfos.find(handle);
		_strokeVBMgr.Retire(oldRent->second);
		_strokeRentInfos.erase(oldRent);
	}
}

bool D3D9LineGeometryTRC::TryGet(UINT_PTR handle, StorkeRenderCall & info) const
{
	auto it = _strokeRentInfos.find(handle);
	if (it != _strokeRentInfos.end())
	{
		info.RenderCall::operator=(_strokeVBMgr.GetDrawCall(it->second));
		return true;
	}
	return false;
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

bool D3D9ResourceManager::TryGet(IResource* res, StorkeRenderCall& rc) const
{
	auto resRef = static_cast<ResourceRef*>(res);
	switch (resRef->GetType())
	{
	case RT_LineGeometry:
		return _lineGeometryTRC.TryGet(resRef->GetHandle(), rc);
	default:
		break;
	}
	return false;
}