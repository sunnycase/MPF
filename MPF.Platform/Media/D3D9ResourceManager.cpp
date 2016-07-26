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

D3D9ResourceManager::D3D9ResourceManager(IDirect3DDevice9* device)
	:_device(device), _vbMgr(device), _lineGeometryTRC(_vbMgr)
{
}

D3D9LineGeometryTRC::D3D9LineGeometryTRC(D3D9VertexBufferManager & vbMgr)
	:_vbMgr(vbMgr)
{
}

namespace
{
	void Transform(std::vector<D3D::Vertex>& vertices, const LineGeometry& geometry)
	{
		vertices.emplace_back(D3D::Vertex
		{
			{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y, 0.f },
			{ 1.f, 1.f, 0.f, 1.f}
		});
		vertices.emplace_back(D3D::Vertex
		{
			{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y, 0.f },
			{ 1.f, 1.f, 0.f, 1.f }
		});
		vertices.emplace_back(D3D::Vertex
		{
			{ 400, 200, 0.f },
			{ 1.f, 1.f, 0.f, 1.f }
		});
	}
}

void D3D9LineGeometryTRC::Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	static std::vector<D3D::Vertex> vertices;
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);
		vertices.clear();
		Transform(vertices, source);
		auto rent = _vbMgr.Allocate(vertices.size());
		_rentInfos.emplace(handle, rent);
		_vbMgr.Update(rent, 0, vertices.data(), vertices.size());
	}
}

void D3D9LineGeometryTRC::Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	static std::vector<D3D::Vertex> vertices;
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);
		vertices.clear();
		Transform(vertices, source);

		auto& oldRent = _rentInfos.find(handle)->second;
		if (oldRent.length != vertices.size())
		{
			_vbMgr.Retire(oldRent);
			oldRent = _vbMgr.Allocate(vertices.size());
		}
		_vbMgr.Update(oldRent, 0, vertices.data(), vertices.size());
	}
}

void D3D9LineGeometryTRC::Remove(const std::vector<UINT_PTR>& handles)
{
	for (auto&& handle : handles)
	{
		auto oldRent = _rentInfos.find(handle);
		_vbMgr.Retire(oldRent->second);
		_rentInfos.erase(oldRent);
	}
}

bool D3D9LineGeometryTRC::TryGet(UINT_PTR handle, RenderCall & info) const
{
	auto it = _rentInfos.find(handle);
	if (it != _rentInfos.end())
	{
		info = _vbMgr.GetDrawCall(it->second);
		return true;
	}
	return false;
}

void D3D9ResourceManager::UpdateOverride()
{
	_vbMgr.Upload();
}

namespace
{
	class DrawCallList final : public IDrawCallList
	{
	public:
		DrawCallList(IDirect3DDevice9* device, D3D9ResourceManager* resMgr)
			:_device(device), _resMgr(resMgr)
		{

		}

		// 通过 IDrawCallList 继承
		virtual void PushDrawCall(IResource * resource) override
		{
			RenderCall rc;
			if (_resMgr->TryGet(resource, rc))
				_renderCalls.emplace_back(rc);
		}

		// 通过 IDrawCallList 继承
		virtual void Draw() override
		{
			for (auto&& rc : _renderCalls)
			{
				ThrowIfFailed(_device->SetStreamSource(0, rc.VB.Get(), 0, sizeof(D3D::Vertex)));
				ThrowIfFailed(_device->DrawPrimitive(D3DPT_TRIANGLELIST, rc.StartVertex, rc.VertexCount / 3));
			}
		}
	private:
		ComPtr<IDirect3DDevice9> _device;
		ComPtr<D3D9ResourceManager> _resMgr;
		std::vector<RenderCall> _renderCalls;
	};
}

std::shared_ptr<IDrawCallList> D3D9ResourceManager::CreateDrawCallList()
{
	return std::make_shared<DrawCallList>(_device.Get(), this);
}

bool D3D9ResourceManager::TryGet(IResource* res, RenderCall& rc) const
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