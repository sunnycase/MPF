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

D3D9ResourceManager::D3D9ResourceManager()
{
}

void D3D9LineGeometryTRC::Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);

	}
}

void D3D9LineGeometryTRC::Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container)
{
	for (auto&& handle : handles)
	{
		auto& source = container.FindResource(handle);

	}
}

void D3D9LineGeometryTRC::Remove(const std::vector<UINT_PTR>& handles)
{
	for (auto&& handle : handles)
	{
	}
}
