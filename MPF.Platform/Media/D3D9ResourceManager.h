//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#pragma once
#include "ResourceManagerBase.h"
#include <d3d9.h>
#include "D3D9BufferManager.h"
#include <unordered_map>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class D3D9LineGeometryTRC : public ITransformedResourceContainer<LineGeometry>
{
public:
	D3D9LineGeometryTRC(D3D9VertexBufferManager& strokeVBMgr);

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override;
	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override;
	virtual void Remove(const std::vector<UINT_PTR>& handles) override;
	bool TryGet(UINT_PTR handle, StorkeRenderCall& rc) const;
private:
	D3D9VertexBufferManager& _strokeVBMgr;
	std::unordered_map<UINT_PTR, RentInfo> _strokeRentInfos;
};

class D3D9ResourceManager : public ResourceManagerBase
{
public:
	D3D9ResourceManager(IDirect3DDevice9* device);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;
	bool TryGet(IResource* res, StorkeRenderCall& rc) const;
protected:
	virtual ITransformedResourceContainer<LineGeometry>& GetLineGeometryTRC() noexcept override { return _lineGeometryTRC; }
	virtual void UpdateOverride() override;
private:
	WRL::ComPtr<IDirect3DDevice9> _device;
	D3D9VertexBufferManager _strokeVBMgr;
	D3D9LineGeometryTRC _lineGeometryTRC;
};

END_NS_PLATFORM