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
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeVertex = typename PlatformProvider_t::StrokeVertex;
	using FillVertex = typename PlatformProvider_t::FillVertex;

	using StorkeRenderCall_t = StorkeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;

	TransformedResourceContainer(VertexBufferManager& strokeVBMgr, VertexBufferManager& fillVBMgr)
		:_strokeVBMgr(strokeVBMgr), _fillVBMgr(fillVBMgr)
	{

	}

	virtual void Add(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<StrokeVertex> strokeVertices;
		static std::vector<FillVertex> fillVertices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;

			strokeVertices.clear();
			_platformProvider.Transform(strokeVertices, source);
			auto strokeRent = _strokeVBMgr.Allocate(fillVertices.size());
			_strokeRentInfos.emplace(handle, strokeRent);
			_strokeVBMgr.Update(strokeRent, 0, fillVertices.data(), fillVertices.size());

			fillVertices.clear();
			_platformProvider.Transform(fillVertices, source);
			auto fillRent = _fillVBMgr.Allocate(fillVertices.size());
			_fillRentInfos.emplace(handle, fillRent);
			_fillVBMgr.Update(fillRent, 0, fillVertices.data(), fillVertices.size());
		}
	}

	virtual void Update(const std::vector<UINT_PTR>& handles, const ResourceContainer<T>& container) override
	{
		static std::vector<StrokeVertex> strokeVertices;
		static std::vector<FillVertex> fillVertices;
		for (auto&& handle : handles)
		{
			T const* refer;
			if (!container.TryFindResource(handle, refer))continue;
			const auto& source = *refer;

			strokeVertices.clear();
			_platformProvider.Transform(strokeVertices, source);
			auto& strokeRent = _strokeRentInfos.find(handle)->second;
			if (strokeRent.length != strokeVertices.size())
			{
				_strokeVBMgr.Retire(strokeRent);
				strokeRent = _strokeVBMgr.Allocate(strokeVertices.size());
			}
			_strokeVBMgr.Update(strokeRent, 0, strokeVertices.data(), strokeVertices.size());

			fillVertices.clear();
			_platformProvider.Transform(fillVertices, source);
			auto& fillRent = _fillRentInfos.find(handle)->second;
			if (fillRent.length != fillVertices.size())
			{
				_fillVBMgr.Retire(fillRent);
				fillRent = _fillVBMgr.Allocate(fillVertices.size());
			}
			_fillVBMgr.Update(fillRent, 0, fillVertices.data(), fillVertices.size());
		}
	}

	virtual void Remove(const std::vector<UINT_PTR>& handles) override
	{
		for (auto&& handle : handles)
		{
			auto strokeRent = _strokeRentInfos.find(handle);
			if (strokeRent != _strokeRentInfos.end())
			{
				_strokeVBMgr.Retire(strokeRent->second);
				_strokeRentInfos.erase(strokeRent);
			}

			auto fillRent = _fillRentInfos.find(handle);
			if (fillRent != _fillRentInfos.end())
			{
				_fillVBMgr.Retire(fillRent->second);
				_fillRentInfos.erase(fillRent);
			}
		}
	}

	bool TryGet(UINT_PTR handle, StorkeRenderCall_t& rc) const
	{
		auto it = _strokeRentInfos.find(handle);
		if (it != _strokeRentInfos.end())
		{
			static_cast<RenderCall&>(rc) = _strokeVBMgr.GetRenderCall(it->second);
			return true;
		}
		return false;
	}

	bool TryGet(UINT_PTR handle, FillRenderCall_t& rc) const
	{
		auto it = _fillRentInfos.find(handle);
		if (it != _fillRentInfos.end())
		{
			static_cast<RenderCall&>(rc) = _fillVBMgr.GetRenderCall(it->second);
			return true;
		}
		return false;
	}
private:
	PlatformProvider<PId> _platformProvider;
	VertexBufferManager& _strokeVBMgr;
	VertexBufferManager& _fillVBMgr;
	std::unordered_map<UINT_PTR, RentInfo> _strokeRentInfos;
	std::unordered_map<UINT_PTR, RentInfo> _fillRentInfos;
};

END_NS_PLATFORM