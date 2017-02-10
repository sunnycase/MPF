//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "TransformedResourceContainer.h"
#include "ResourceManagerBase.h"
#include "Platform/PlatformProviderTraits.h"
#include "DrawCallList.h"

DEFINE_NS_PLATFORM

#define RM_DECL_GET_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept override { return _trc##T; }

#define RM_DECL_TRC_MEMBER(T) \
TransformedResourceContainer<PId, T> _trc##T

#define RM_CTOR_IMPL1(T) _trc##T##(_strokeVBMgr, _strokeIBMgr, _fillVBMgr, _fillIBMgr)

template<PlatformId PId>
class ResourceManager : public ResourceManagerBase
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeVertex = typename PlatformProvider_t::StrokeVertex;
	using FillVertex = typename PlatformProvider_t::FillVertex;
	using DeviceContext = typename PlatformProvider_t::DeviceContext;
	using DrawCallList = typename PlatformProviderTraits<PId>::DrawCallList;

	using VertexBufferManager = typename PlatformProviderTraits<PId>::VertexBufferManager;
	using IndexBufferManager = typename PlatformProviderTraits<PId>::IndexBufferManager;

	using StrokeRenderCall_t = StrokeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;

	ResourceManager(DeviceContext& deviceContext)
		:_deviceContext(deviceContext),
		_strokeVBMgr(deviceContext, sizeof(StrokeVertex)), _fillVBMgr(deviceContext, sizeof(FillVertex)),
		_strokeIBMgr(deviceContext), _fillIBMgr(deviceContext),
		RM_CTOR_IMPL1(LineGeometry), RM_CTOR_IMPL1(RectangleGeometry), RM_CTOR_IMPL1(PathGeometry),
		RM_CTOR_IMPL1(BoxGeometry3D)
	{

	}

#define RM_TRYGET_IMPL1(T)										 \
	case RT_##T:												 \
		return _trc##T.TryGet(resRef->GetHandle(), rc);

	bool TryGet(IResource* res, StrokeRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_TRYGET_IMPL1(LineGeometry);
			RM_TRYGET_IMPL1(RectangleGeometry);
			RM_TRYGET_IMPL1(PathGeometry);
			RM_TRYGET_IMPL1(BoxGeometry3D);
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
			RM_TRYGET_IMPL1(BoxGeometry3D);
		default:
			break;
		}
		return false;
	}

	decltype(auto) GetVertexBuffer(const StrokeRenderCall_t& renderCall) const noexcept
	{
		return _strokeVBMgr.GetBuffer(renderCall.VB);
	}

	decltype(auto) GetVertexBuffer(const FillRenderCall_t& renderCall) const noexcept
	{
		return _fillVBMgr.GetBuffer(renderCall.VB);
	}

	decltype(auto) GetIndexBuffer(const StrokeRenderCall_t& renderCall) const noexcept
	{
		return _strokeIBMgr.GetBuffer(renderCall.IB);
	}

	decltype(auto) GetIndexBuffer(const FillRenderCall_t& renderCall) const noexcept
	{
		return _fillIBMgr.GetBuffer(renderCall.IB);
	}

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override
	{
		WRL::ComPtr<ResourceManager> me(this);
		auto dcl = std::make_shared<DrawCallList>(_deviceContext, me, rcb);
		dcl->Initialize();
		return dcl;
	}
protected:
	RM_DECL_GET_TRC(LineGeometry);
	RM_DECL_GET_TRC(RectangleGeometry);
	RM_DECL_GET_TRC(PathGeometry);
	RM_DECL_GET_TRC(BoxGeometry3D);

	virtual void UpdateOverride() override
	{
		_strokeVBMgr.Upload();
		_strokeIBMgr.Upload();
		_fillVBMgr.Upload();
		_fillIBMgr.Upload();
	}

	DeviceContext _deviceContext;
private:
	VertexBufferManager _strokeVBMgr;
	VertexBufferManager _fillVBMgr;
	IndexBufferManager _strokeIBMgr;
	IndexBufferManager _fillIBMgr;

	RM_DECL_TRC_MEMBER(LineGeometry);
	RM_DECL_TRC_MEMBER(RectangleGeometry);
	RM_DECL_TRC_MEMBER(PathGeometry);
	RM_DECL_TRC_MEMBER(BoxGeometry3D);
};

END_NS_PLATFORM