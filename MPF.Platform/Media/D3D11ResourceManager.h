//
// MPF Platform
// D3D11 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#pragma once
#include "ResourceManagerBase.h"
#include <d3d11.h>
#include "D3D11BufferManager.h"
#include <unordered_map>
#include "D3D11Vertex.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

template<typename T>
class D3D11TransformedResourceContainerBase : public ITransformedResourceContainer<T>
{
public:
	D3D11TransformedResourceContainerBase(D3D11VertexBufferManager& strokeVBMgr)
		:_strokeVBMgr(strokeVBMgr) {}

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<D3D11::StrokeVertex> vertices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;
			vertices.clear();
			Transform(vertices, source);
			auto rent = _strokeVBMgr.Allocate(vertices.size());
			_strokeRentInfos.emplace(handle, rent);
			_strokeVBMgr.Update(rent, 0, vertices.data(), vertices.size());
		}
	}

	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<D3D11::StrokeVertex> vertices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;
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
			if (oldRent != _strokeRentInfos.end())
			{
				_strokeVBMgr.Retire(oldRent->second);
				_strokeRentInfos.erase(oldRent);
			}
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
	D3D11VertexBufferManager& _strokeVBMgr;
	std::unordered_map<UINT_PTR, RentInfo> _strokeRentInfos;
};

template<typename T>
class D3D11GeometryTRC final : public D3D11TransformedResourceContainerBase<T>
{
public:
	using D3D11TransformedResourceContainerBase::D3D11TransformedResourceContainerBase;
private:
};

void Transform(std::vector<D3D11::StrokeVertex>& vertices, const LineGeometry& geometry);
void Transform(std::vector<D3D11::StrokeVertex>& vertices, const RectangleGeometry& geometry);
void Transform(std::vector<D3D11::StrokeVertex>& vertices, const PathGeometry& geometry);

#define DECL_GET_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept override { return _trc##T; }

#define DECL_TRC_MEMBER(T) \
D3D11GeometryTRC<T> _trc##T
class D3D11ResourceManager : public ResourceManagerBase
{
public:
	D3D11ResourceManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext, SwapChainUpdateContext& updateContext);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;
	bool TryGet(IResource* res, StorkeRenderCall& rc) const;

	ID3D11Buffer* GetVertexBuffer(const StorkeRenderCall& renderCall) const noexcept
	{
		return _strokeVBMgr.GetVertexBuffer(renderCall);
	}
protected:
	DECL_GET_TRC(LineGeometry);
	DECL_GET_TRC(RectangleGeometry);
	DECL_GET_TRC(PathGeometry);
	virtual void UpdateOverride() override;
private:
	WRL::ComPtr<ID3D11Device> _device;
	WRL::ComPtr<ID3D11DeviceContext> _deviceContext;
	D3D11VertexBufferManager _strokeVBMgr;
	SwapChainUpdateContext _updateContext;
	DECL_TRC_MEMBER(LineGeometry);
	DECL_TRC_MEMBER(RectangleGeometry);
	DECL_TRC_MEMBER(PathGeometry);
};

END_NS_PLATFORM