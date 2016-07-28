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

template<typename T>
class D3D9TransformedResourceContainerBase : public ITransformedResourceContainer<LineGeometry>
{
public:
	D3D9TransformedResourceContainerBase(D3D9VertexBufferManager& strokeVBMgr)
		:_strokeVBMgr(strokeVBMgr) {}

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override
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

	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<LineGeometry>& container) override
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

	virtual void Remove(const std::vector<UINT_PTR>& handles) override
	{
		for (auto&& handle : handles)
		{
			auto oldRent = _strokeRentInfos.find(handle);
			_strokeVBMgr.Retire(oldRent->second);
			_strokeRentInfos.erase(oldRent);
		}
	}

	bool TryGet(UINT_PTR handle, StorkeRenderCall& rc) const
	{
		auto it = _strokeRentInfos.find(handle);
		if (it != _strokeRentInfos.end())
		{
			static_cast<RenderCall&>(rc) = _strokeVBMgr.GetDrawCall(it->second);
			return true;
		}
		return false;
	}
private:
	D3D9VertexBufferManager& _strokeVBMgr;
	std::unordered_map<UINT_PTR, RentInfo> _strokeRentInfos;
};

void Transform(std::vector<D3D::StrokeVertex>& vertices, const LineGeometry& geometry);

class D3D9LineGeometryTRC final : public D3D9TransformedResourceContainerBase<LineGeometry>
{
public:
	using D3D9TransformedResourceContainerBase::D3D9TransformedResourceContainerBase;
private:
};

class D3D9StreamGeometryTRC final : public D3D9TransformedResourceContainerBase<StreamGeometry>
{
public:
	using D3D9TransformedResourceContainerBase::D3D9TransformedResourceContainerBase;
private:
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