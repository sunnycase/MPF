//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#include "stdafx.h"
#include "D3D9ResourceManager.h"
using namespace WRL;
using namespace NS_PLATFORM;

D3D9ResourceManager::D3D9ResourceManager(IDirect3DDevice9* device)
	:_vbMgr(device), _lineGeometryTRC(_vbMgr)
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
			{ geometry.Data.StartPoint.X, geometry.Data.StartPoint.Y, 0.5f },
			{ 1.f, 1.f, 0.f, 1.f}
		});
		vertices.emplace_back(D3D::Vertex
		{
			{ geometry.Data.EndPoint.X, geometry.Data.EndPoint.Y, 0.5f },
			{ 1.f, 1.f, 0.f, 1.f }
		});
		vertices.emplace_back(D3D::Vertex
		{
			{ 50, 50, 0.5f },
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

void D3D9ResourceManager::UpdateOverride()
{
	_vbMgr.Upload();
}
