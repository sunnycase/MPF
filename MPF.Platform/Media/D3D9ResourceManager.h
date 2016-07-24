//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#pragma once
#include "ResourceManagerBase.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9LineGeometryTRC : public ITransformedResourceContainer<LineGeometry>
{
public:
	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override;
	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override;
	virtual void Remove(const std::vector<UINT_PTR>& handles) override;
private:
};

class D3D9ResourceManager : public ResourceManagerBase
{
public:
	D3D9ResourceManager();

protected:
	virtual ITransformedResourceContainer<LineGeometry>& GetLineGeometryTRC() noexcept override { return _lineGeometryTRC; }
private:
	D3D9LineGeometryTRC _lineGeometryTRC;
};

END_NS_PLATFORM