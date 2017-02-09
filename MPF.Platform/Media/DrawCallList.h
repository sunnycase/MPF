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
		PlayRenderCallArgs<PId> args{ _deviceContext, _resMgr, modelTransform, _strokeRenderCalls, _fillRenderCalls };
		_platformProvider.PlayRenderCall(args);
	}
private:
	template<bool AddResDependent>
	void Update()
	{
		_strokeRenderCalls.clear();
		_fillRenderCalls.clear();
		for (auto&& geoRef : _rcb->GetGeometries())
		{
			PushGeometryDrawCall(geoRef.Geometry.Get(), geoRef.Pen.Get(), geoRef.Brush.Get(), geoRef.Transform);
			if (AddResDependent)
			{
				auto me = shared_from_this();
				_resMgr->AddDependentDrawCallList(me, geoRef.Geometry.Get());
				_resMgr->AddDependentDrawCallList(me, geoRef.Pen.Get());
				_resMgr->AddDependentDrawCallList(me, geoRef.Brush.Get());
			}
		}
	}

	void PushGeometryDrawCall(IResource* resource, IResource* pen, IResource* brush, const DirectX::XMFLOAT4X4 transform)
	{
		if (pen)
		{
			StrokeRenderCall_t rc;
			auto& penObj = _resMgr->GetPen(static_cast<ResourceRef*>(pen)->GetHandle());
			rc.Thickness = penObj.Thickness;
			if (penObj.Brush)
			{
				auto& brushObj = _resMgr->GetBrush(penObj.Brush->GetHandle());
				if (typeid(brushObj) == typeid(Brush&))
				{
					auto& color = static_cast<SolidColorBrush&>(brushObj).Color;
					rc.Color[0] = color.R;
					rc.Color[1] = color.G;
					rc.Color[2] = color.B;
					rc.Color[3] = color.A;
				}
				if (_resMgr->TryGet(resource, rc))
				{
					if (!_platformProvider.IsNopRenderCall(rc))
					{
						rc.Transform = transform;
						_strokeRenderCalls.emplace_back(rc);
					}
				}
				else
				{
					//assert(false && "Geometry not found.");
				}
			}
		}

		if (brush)
		{
			FillRenderCall_t rc;
			auto& brushObj = _resMgr->GetBrush(static_cast<ResourceRef*>(brush)->GetHandle());
			if (typeid(brushObj) == typeid(Brush&))
			{
				auto& color = static_cast<SolidColorBrush&>(brushObj).Color;
				rc.Color[0] = color.R;
				rc.Color[1] = color.G;
				rc.Color[2] = color.B;
				rc.Color[3] = color.A;
			}
			if (_resMgr->TryGet(resource, rc))
			{
				if (!_platformProvider.IsNopRenderCall(rc))
				{
					rc.Transform = transform;
					_fillRenderCalls.emplace_back(rc);
				}
			}
			else
			{
				//assert(false && "Geometry not found.");
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
};

END_NS_PLATFORM