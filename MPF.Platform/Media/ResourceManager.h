//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "TransformedResourceContainer.h"
#include "ResourceManagerBase.h"

DEFINE_NS_PLATFORM

#define RM_DECL_GET_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept override { return _trc##T; }

#define RM_DECL_TRC_MEMBER(T) \
TransformedResourceContainer<PId, T> _trc##T

#define RM_CTOR_IMPL1(T) _trc##T##(_strokeVBMgr)

template<PlatformId PId>
class ResourceManager : public ResourceManagerBase
{
	using PlatformProvider_t = PlatformProvider<PId>;
	using VertexBufferManager = typename PlatformProvider_t::VertexBufferManager;
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeVertex = typename PlatformProvider_t::StrokeVertex;
	using FillVertex = typename PlatformProvider_t::FillVertex;

	using StorkeRenderCall_t = StorkeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;
public:
	template<class TVBUploadCaller, class StrokeTuple, class FillTuple>
	ResourceManager(TVBUploadCaller&& vbUploadCaller, StrokeTuple&& strokeVBTuple, FillTuple&& fillVBTuple)
		:ResourceManager(std::forward<TVBUploadCaller>(vbUploadCaller), std::forward<StrokeTuple>(strokeVBTuple), std::make_index_sequence<std::tuple_size_v<std::decay_t<StrokeTuple>>>{},
			std::forward<FillTuple>(fillVBTuple), std::make_index_sequence<std::tuple_size_v<std::decay_t<FillTuple>>>{})
	{

	}

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override
	{
		return nullptr;
	}

#define RM_TRYGET_IMPL1(T)										 \
	case RT_##T:												 \
		return _trc##T.TryGet(resRef->GetHandle(), rc);

	bool TryGet(IResource* res, StorkeRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_TRYGET_IMPL1(LineGeometry);
			RM_TRYGET_IMPL1(RectangleGeometry);
			RM_TRYGET_IMPL1(PathGeometry);
		default:
			break;
		}
		return false;
	}

	bool TryGet(IResource* res, FillRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_TRYGET_IMPL1(LineGeometry);
			RM_TRYGET_IMPL1(RectangleGeometry);
			RM_TRYGET_IMPL1(PathGeometry);
		default:
			break;
		}
		return false;
	}

	decltype(auto) GetVertexBuffer(const StorkeRenderCall_t& renderCall) const noexcept
	{
		return _strokeVBMgr.GetVertexBuffer(renderCall);
	}

	decltype(auto) GetVertexBuffer(const FillRenderCall_t& renderCall) const noexcept
	{
		return _fillVBMgr.GetVertexBuffer(renderCall);
	}
private:
	template<class TVBUploadCaller, class StrokeTuple, std::size_t... IStrokeTuple, class FillTuple, std::size_t... IFillTuple>
	ResourceManager(TVBUploadCaller&& vbUploadCaller, StrokeTuple&& strokeVBTuple, FillTuple&& fillVBTuple, std::index_sequence<IStrokeTuple...>, std::index_sequence<IFillTuple...>)
		:_vbUploadCaller(std::forward<TVBUploadCaller>(vbUploadCaller)), _strokeVBMgr(std::get<IStrokeTuple>(std::forward<StrokeTuple>(t))...), 
		_fillVBMgr(std::get<IFillTuple>(std::forward<FillTuple>(t))...)
	{

	}
protected:
	RM_DECL_GET_TRC(LineGeometry);
	RM_DECL_GET_TRC(RectangleGeometry);
	RM_DECL_GET_TRC(PathGeometry);

	virtual void UpdateOverride() override
	{
		_vbUploadCaller(_strokeVBMgr);
		_vbUploadCaller(_fillVBMgr);
	}

	VertexBufferManager _strokeVBMgr;
	VertexBufferManager _fillVBMgr;
private:
	std::function<void(VertexBufferManager&)> _vbUploadCaller;
	RM_DECL_TRC_MEMBER(LineGeometry);
	RM_DECL_TRC_MEMBER(RectangleGeometry);
	RM_DECL_TRC_MEMBER(PathGeometry);
};

END_NS_PLATFORM