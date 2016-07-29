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
class D3D9TransformedResourceContainerBase : public ITransformedResourceContainer<T>
{
public:
	D3D9TransformedResourceContainerBase(D3D9VertexBufferManager& strokeVBMgr)
		:_strokeVBMgr(strokeVBMgr) {}

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
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

	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
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

template<typename T>
class D3D9GeometryTRC final : public D3D9TransformedResourceContainerBase<T>
{
public:
	using D3D9TransformedResourceContainerBase::D3D9TransformedResourceContainerBase;
private:
};

void Transform(std::vector<D3D::StrokeVertex>& vertices, const LineGeometry& geometry);
void Transform(std::vector<D3D::StrokeVertex>& vertices, const RectangleGeometry& geometry);

#define DECL_GET_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept override { return _trc##T; }

#define DECL_TRC_MEMBER(T) \
D3D9GeometryTRC<T> _trc##T
class D3D9ResourceManager : public ResourceManagerBase
{
public:
	D3D9ResourceManager(IDirect3DDevice9* device);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;
	bool TryGet(IResource* res, StorkeRenderCall& rc) const;
protected:
	DECL_GET_TRC(LineGeometry);
	DECL_GET_TRC(RectangleGeometry);
	virtual void UpdateOverride() override;
private:
	WRL::ComPtr<IDirect3DDevice9> _device;
	D3D9VertexBufferManager _strokeVBMgr;
	DECL_TRC_MEMBER(LineGeometry);
	DECL_TRC_MEMBER(RectangleGeometry);
};

END_NS_PLATFORM