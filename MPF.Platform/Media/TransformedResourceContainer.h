//
// MPF Platform
// Transformed Resource Container
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "../../inc/common.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include "Platform/PlatformProvider.h"
#include "Platform/PlatformProviderTraits.h"
#include "Geometry.h"

DEFINE_NS_PLATFORM

template<PlatformId PId, typename T>
class TransformedResourceContainer : public ITransformedResourceContainer<T>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using VertexBufferManager = typename PlatformProviderTraits<PId>::VertexBufferManager;
	using IndexBufferManager = typename PlatformProviderTraits<PId>::IndexBufferManager;
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeVertex = typename PlatformProvider_t::StrokeVertex;
	using FillVertex = typename PlatformProvider_t::FillVertex;

	using StrokeRenderCall_t = StrokeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;

	TransformedResourceContainer(VertexBufferManager& strokeVBMgr, IndexBufferManager& strokeIBMgr, VertexBufferManager& fillVBMgr, IndexBufferManager& fillIBMgr)
		:_strokeVBMgr(strokeVBMgr), _strokeIBMgr(strokeIBMgr), _fillVBMgr(fillVBMgr), _fillIBMgr(fillIBMgr)
	{

	}

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<StrokeVertex> strokeVertices;
		static std::vector<size_t> strokeIndices;
		static std::vector<FillVertex> fillVertices;
		static std::vector<size_t> fillIndices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;

			strokeVertices.clear();
			strokeIndices.clear();
			_platformProvider.Transform(strokeVertices, strokeIndices, source);
			auto strokeVBRent = _strokeVBMgr.Allocate(strokeVertices.size());
			auto strokeIBRent = _strokeIBMgr.Allocate(strokeIndices.size());
			_strokeRentInfos.emplace(handle, GeometryRentInfo{ strokeVBRent, strokeIBRent });
			_strokeVBMgr.Update(strokeVBRent, 0, strokeVertices.data(), strokeVertices.size());
			_strokeIBMgr.Update(strokeIBRent, 0, strokeIndices.data(), strokeIndices.size());

			fillVertices.clear();
			fillIndices.clear();
			_platformProvider.Transform(fillVertices, fillIndices, source);
			auto fillVBRent = _fillVBMgr.Allocate(fillVertices.size());
			auto fillIBRent = _fillIBMgr.Allocate(fillIndices.size());
			_fillRentInfos.emplace(handle, GeometryRentInfo{ fillVBRent, fillIBRent });
			_fillVBMgr.Update(fillVBRent, 0, fillVertices.data(), fillVertices.size());
			_fillIBMgr.Update(fillIBRent, 0, fillIndices.data(), fillIndices.size());
		}
	}

	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<StrokeVertex> strokeVertices;
		static std::vector<size_t> strokeIndices;
		static std::vector<FillVertex> fillVertices;
		static std::vector<size_t> fillIndices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;

			strokeVertices.clear();
			strokeIndices.clear();
			_platformProvider.Transform(strokeVertices, strokeIndices, source);
			auto& strokeRent = _strokeRentInfos.find(handle)->second;
			if (strokeRent.Vertices.length != strokeVertices.size())
			{
				_strokeVBMgr.Retire(strokeRent.Vertices);
				strokeRent.Vertices = _strokeVBMgr.Allocate(strokeVertices.size());
			}
			_strokeVBMgr.Update(strokeRent.Vertices, 0, strokeVertices.data(), strokeVertices.size());
			if (strokeRent.Indices.length != strokeIndices.size())
			{
				_strokeIBMgr.Retire(strokeRent.Indices);
				strokeRent.Indices = _strokeIBMgr.Allocate(strokeIndices.size());
			}
			_strokeIBMgr.Update(strokeRent.Indices, 0, strokeIndices.data(), strokeIndices.size());

			fillVertices.clear();
			fillIndices.clear();
			_platformProvider.Transform(fillVertices, fillIndices, source);
			auto& fillRent = _fillRentInfos.find(handle)->second;
			if (fillRent.Vertices.length != fillVertices.size())
			{
				_fillVBMgr.Retire(fillRent.Vertices);
				fillRent.Vertices = _fillVBMgr.Allocate(fillVertices.size());
			}
			_fillVBMgr.Update(fillRent.Vertices, 0, fillVertices.data(), fillVertices.size());
			if (fillRent.Indices.length != fillIndices.size())
			{
				_fillIBMgr.Retire(fillRent.Indices);
				fillRent.Indices = _fillIBMgr.Allocate(fillIndices.size());
			}
			_fillIBMgr.Update(fillRent.Indices, 0, fillIndices.data(), fillIndices.size());
		}
	}

	virtual void Remove(const std::vector<UINT_PTR>& handles) override
	{
		for (auto&& handle : handles)
		{
			auto strokeRent = _strokeRentInfos.find(handle);
			if (strokeRent != _strokeRentInfos.end())
			{
				_strokeVBMgr.Retire(strokeRent->second.Vertices);
				_strokeIBMgr.Retire(strokeRent->second.Indices);
				_strokeRentInfos.erase(strokeRent);
			}

			auto fillRent = _fillRentInfos.find(handle);
			if (fillRent != _fillRentInfos.end())
			{
				_fillVBMgr.Retire(fillRent->second.Vertices);
				_fillIBMgr.Retire(fillRent->second.Indices);
				_fillRentInfos.erase(fillRent);
			}
		}
	}

	bool TryGet(UINT_PTR handle, StrokeRenderCall_t& rc) const
	{
		auto it = _strokeRentInfos.find(handle);
		if (it != _strokeRentInfos.end())
		{
			_strokeVBMgr.GetRenderCall(it->second.Vertices, rc);
			_strokeIBMgr.GetRenderCall(it->second.Indices, rc);
			return true;
		}
		return false;
	}

	bool TryGet(UINT_PTR handle, FillRenderCall_t& rc) const
	{
		auto it = _fillRentInfos.find(handle);
		if (it != _fillRentInfos.end())
		{
			_fillVBMgr.GetRenderCall(it->second.Vertices, rc);
			_fillIBMgr.GetRenderCall(it->second.Indices, rc);
			return true;
		}
		return false;
	}
private:
	PlatformProvider<PId> _platformProvider;
	VertexBufferManager& _strokeVBMgr;
	IndexBufferManager& _strokeIBMgr;
	VertexBufferManager& _fillVBMgr;
	IndexBufferManager& _fillIBMgr;
	std::unordered_map<UINT_PTR, GeometryRentInfo> _strokeRentInfos;
	std::unordered_map<UINT_PTR, GeometryRentInfo> _fillRentInfos;
};

END_NS_PLATFORM