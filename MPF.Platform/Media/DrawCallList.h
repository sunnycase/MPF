//
// MPF Platform
// Draw Call List
// 作者：SunnyCase
// 创建时间：2016-12-26
//
#pragma once
#include "ResourceManager.h"
#include "RenderCommandBuffer.h"
#include "Platform/PlatformProviderTraits.h"

DEFINE_NS_PLATFORM

template<PlatformId PId>
class DrawCallList : public IDrawCallList
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using ResourceManager = typename PlatformProviderTraits<PId>::ResourceManager;
	using DeviceContext = typename PlatformProvider_t::DeviceContext;
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeRenderCall_t = StrokeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;
	using Fill3DRenderCall_t = Fill3DRenderCall<RenderCall>;

	DrawCallList(DeviceContext& deviceContext, ResourceManager& resMgr, RenderCommandBuffer* rcb)
		:_deviceContext(deviceContext), _resMgr(resMgr), _rcb(rcb)
	{
	}

	void Initialize()
	{
		Update<true>();
	}

	virtual void Update() override
	{
		Update<false>();
	}

	virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) override
	{
		PlayRenderCallArgs<PId> args{ _deviceContext, _resMgr, modelTransform, _strokeRenderCalls, _fillRenderCalls, _fill3DRenderCalls };
		_platformProvider.PlayRenderCall(args);
	}
private:
	template<bool AddResDependent>
	void Update()
	{
		_strokeRenderCalls.clear();
		_fillRenderCalls.clear();
		for (auto&& geoRef : _rcb->GetCommands())
		{
			PushGeometryDrawCall(geoRef.Geometry.Get(), geoRef.Pen.Get(), geoRef.Brush.Get(), geoRef.Transform);
			if (AddResDependent)
			{
				auto me = shared_from_this();
				if (geoRef.Geometry)
					_resMgr->AddDependentDrawCallList(me, geoRef.Geometry.Get());
				if (geoRef.Pen)
					_resMgr->AddDependentDrawCallList(me, geoRef.Pen.Get());
				if (geoRef.Brush)
					_resMgr->AddDependentDrawCallList(me, geoRef.Brush.Get());
			}
		}

		_fill3DRenderCalls.clear();
		for (auto&& geoRef : _rcb->GetCommand3Ds())
		{
			PushGeometry3DDrawCall(geoRef.Geometry3D.Get(), geoRef.Material.Get(), geoRef.Transform);
			if (AddResDependent)
			{
				auto me = shared_from_this();
				if (geoRef.Geometry3D)
					_resMgr->AddDependentDrawCallList(me, geoRef.Geometry3D.Get());
				if (geoRef.Material)
					_resMgr->AddDependentDrawCallList(me, geoRef.Material.Get());
			}
		}
	}

	void PushGeometryDrawCall(IResource* resource, IResource* pen, IResource* brush, const DirectX::XMFLOAT4X4 transform)
	{
		if (pen)
		{
			StrokeRenderCall_t rc;
			if (!_resMgr->PopulateRenderCallWithGeometry(resource, rc))
				;//assert(false && "Geometry not found.");
			else
			{
				if (!_resMgr->PopulateRenderCallWithPen(pen, rc))
					;//assert(false && "Brush not found.");
				else if (!_platformProvider.IsNopRenderCall(rc))
				{
					rc.Transform = transform;
					_strokeRenderCalls.emplace_back(rc);
				}
			}
		}

		if (brush)
		{
			FillRenderCall_t rc;
			if (!_resMgr->PopulateRenderCallWithGeometry(resource, rc))
				;//assert(false && "Geometry not found.");
			else
			{
				if (!_resMgr->PopulateRenderCallWithBrush(brush, rc))
					;//assert(false && "Brush not found.");
				else if (!_platformProvider.IsNopRenderCall(rc))
				{
					rc.Transform = transform;
					_fillRenderCalls.emplace_back(rc);
				}
			}
		}
	}

	void PushGeometry3DDrawCall(IResource* resource, IResource* material, const DirectX::XMFLOAT4X4 transform)
	{
		if (material)
		{
			Fill3DRenderCall_t rc;
			if (!_resMgr->PopulateRenderCallWithGeometry3D(resource, rc))
				;//assert(false && "Geometry not found.");
			else
			{
				if (!_resMgr->PopulateRenderCallWithMaterial(material, rc))
					;//assert(false && "Brush not found.");
				else if (!_platformProvider.IsNopRenderCall(rc))
				{
					rc.Transform = transform;
					_fill3DRenderCalls.emplace_back(rc);
				}
			}
		}
	}
private:
	PlatformProvider_t _platformProvider;
	DeviceContext _deviceContext;
	ResourceManager _resMgr;
	WRL::ComPtr<RenderCommandBuffer> _rcb;

	std::vector<StrokeRenderCall_t> _strokeRenderCalls;
	std::vector<FillRenderCall_t> _fillRenderCalls;
	std::vector<Fill3DRenderCall_t> _fill3DRenderCalls;
};

END_NS_PLATFORM