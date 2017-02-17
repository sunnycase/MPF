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

#define RM_DECL_GET_GEOMETRY_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##StrokeTRC() noexcept override { return _trc##T##Stroke; } \
virtual ITransformedResourceContainer<T>& Get##T##FillTRC() noexcept override { return _trc##T##Fill; }

#define RM_DECL_GET_GEOMETRY3D_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##FillTRC() noexcept override { return _trc##T##Fill; }

#define RM_DECL_GET_DEVICE_TRC(T)	\
virtual ITransformedResourceContainer<T>& Get##T##TRC() noexcept override { return _trc##T; }


#define RM_DECL_GEOMETRY_TRC_MEMBER(T) \
TransformedGeometryResourceContainer<PId, T, StrokeVertex, StrokeRenderCall_t> _trc##T##Stroke; \
TransformedGeometryResourceContainer<PId, T, FillVertex, FillRenderCall_t> _trc##T##Fill

#define RM_DECL_GEOMETRY3D_TRC_MEMBER(T) \
TransformedGeometryResourceContainer<PId, T, Fill3DVertex, Fill3DRenderCall_t> _trc##T##Fill

#define RM_DECL_TEXTURE_TRC_MEMBER(T) \
TransformedTextureResourceContainer<PId, T, BrushRenderCall> _trc##T

#define RM_DECL_SAMPLER_TRC_MEMBER(T) \
TransformedSamplerResourceContainer<PId, T, BrushRenderCall> _trc##T

#define RM_DECL_SHADERSGROUP_TRC_MEMBER(T) \
TransformedShadersGroupResourceContainer<PId, T, MaterialRenderCall> _trc##T

#define RM_DECL_SHADERPARAMETERS_TRC_MEMBER(T) \
TransformedShaderParametersResourceContainer<PId, MaterialRenderCall> _trc##T

#define RM_DECL_DEPENDENT_TRC_MEMBER(T) \
Transformed##T##ResourceContainer<PId, ##T##RenderCall> _trc##T

#define RM_DECL_NO_DEPENDENT_TRC_MEMBER(T) \
Transformed##T##ResourceContainer<PId> _trc##T


#define RM_CTOR_GEOMETRY_IMPL(T) _trc##T##Stroke(RenderCallAwareness::Stroke, _strokeVBMgr, _strokeIBMgr), _trc##T##Fill(RenderCallAwareness::Fill, _fillVBMgr, _fillIBMgr)
#define RM_CTOR_GEOMETRY3D_IMPL(T) _trc##T##Fill(RenderCallAwareness::Fill3D, _fill3DVBMgr, _fill3DIBMgr)
#define RM_CTOR_TEXTURE_IMPL(T) _trc##T(RenderCallAwareness::Texture, _textureBufferMgr)
#define RM_CTOR_SAMPLER_IMPL(T) _trc##T(RenderCallAwareness::Sampler, _samplerBufferMgr)
#define RM_CTOR_SHADERSGROUP_IMPL(T) _trc##T(RenderCallAwareness::ShadersGroup, _shaderBufferMgr)
#define RM_CTOR_SHADERPARAMETERS_IMPL(T) _trc##T(RenderCallAwareness::ShaderParameters)
#define RM_CTOR_DEPENDENT_IMPL(T) _trc##T(RenderCallAwareness::T)
#define RM_CTOR_NO_DEPENDENT_IMPL(T) _trc##T(RenderCallAwareness::None)

template<PlatformId PId>
class ResourceManager : public ResourceManagerBase
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using RenderCall = typename PlatformProvider_t::RenderCall;
	using StrokeVertex = typename PlatformProvider_t::StrokeVertex;
	using FillVertex = typename PlatformProvider_t::FillVertex;
	using Fill3DVertex = typename PlatformProvider_t::Fill3DVertex;
	using DeviceContext = typename PlatformProvider_t::DeviceContext;
	using DrawCallList = typename PlatformProviderTraits<PId>::DrawCallList;

	using VertexBufferManager = typename PlatformProviderTraits<PId>::VertexBufferManager;
	using IndexBufferManager = typename PlatformProviderTraits<PId>::IndexBufferManager;
	using TextureBufferManager = typename PlatformProviderTraits<PId>::TextureBufferManager;
	using SamplerBufferManager = typename PlatformProviderTraits<PId>::SamplerBufferManager;
	using ShaderBufferManager = typename PlatformProviderTraits<PId>::ShaderBufferManager;

	using BrushRenderCall = typename PlatformProvider_t::BrushRenderCall;
	using PenRenderCall = typename PlatformProvider_t::PenRenderCall;
	using MaterialRenderCall = typename PlatformProvider_t::MaterialRenderCall;

	using StrokeRenderCall_t = StrokeRenderCall<RenderCall>;
	using FillRenderCall_t = FillRenderCall<RenderCall>;
	using Fill3DRenderCall_t = Fill3DRenderCall<RenderCall>;

	ResourceManager(DeviceContext& deviceContext)
		:_deviceContext(deviceContext),
		_strokeVBMgr(deviceContext, sizeof(StrokeVertex)), _fillVBMgr(deviceContext, sizeof(FillVertex)),
		_strokeIBMgr(deviceContext), _fillIBMgr(deviceContext),
		_fill3DVBMgr(deviceContext, sizeof(Fill3DVertex)), _fill3DIBMgr(deviceContext),
		_textureBufferMgr(deviceContext), _samplerBufferMgr(deviceContext),
		_shaderBufferMgr(deviceContext),
		RM_CTOR_GEOMETRY_IMPL(LineGeometry), RM_CTOR_GEOMETRY_IMPL(RectangleGeometry), RM_CTOR_GEOMETRY_IMPL(PathGeometry),
		RM_CTOR_GEOMETRY3D_IMPL(BoxGeometry3D), RM_CTOR_GEOMETRY3D_IMPL(MeshGeometry3D),
		RM_CTOR_TEXTURE_IMPL(SolidColorTexture),
		RM_CTOR_SAMPLER_IMPL(Sampler), RM_CTOR_SHADERSGROUP_IMPL(ShadersGroup), RM_CTOR_SHADERPARAMETERS_IMPL(ShaderParameters),
		RM_CTOR_DEPENDENT_IMPL(Brush), RM_CTOR_DEPENDENT_IMPL(Pen), RM_CTOR_DEPENDENT_IMPL(Material),
		RM_CTOR_NO_DEPENDENT_IMPL(Camera)
	{

	}

#define RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(T, Type)			 \
	case RT_##T:												 \
		return _trc##T##Type.PopulateRenderCall(resRef->GetHandle(), rc);

	bool PopulateRenderCallWithGeometry(IResource* res, StrokeRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(LineGeometry, Stroke);
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(RectangleGeometry, Stroke);
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(PathGeometry, Stroke);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithGeometry(IResource* res, FillRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(LineGeometry, Fill);
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(RectangleGeometry, Fill);
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(PathGeometry, Fill);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithGeometry3D(IResource* res, Fill3DRenderCall_t& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(BoxGeometry3D, Fill);
			RM_POPULATE_GEOMETRY_RENDERCALL_IMPL1(MeshGeometry3D, Fill);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

#define RM_POPULATE_DEVICE_RES_RENDERCALL_IMPL1(T)			     \
	case RT_##T:												 \
		return _trc##T.PopulateRenderCall(resRef->GetHandle(), rc);

	bool PopulateRenderCallWithTexture(IResource* res, BrushRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEVICE_RES_RENDERCALL_IMPL1(SolidColorTexture);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithSampler(IResource* res, BrushRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEVICE_RES_RENDERCALL_IMPL1(Sampler);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

#define RM_POPULATE_DEPENDENT_RES_RENDERCALL_IMPL1(T)			     \
	case RT_##T:												 \
		return _trc##T.PopulateRenderCall(*this, resRef->GetHandle(), rc);

	bool PopulateRenderCallWithBrush(IResource* res, BrushRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEPENDENT_RES_RENDERCALL_IMPL1(Brush);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithBrush(IResource* res, FillRenderCall_t& rc) const
	{
		BrushRenderCall brc{};
		if (PopulateRenderCallWithBrush(res, brc))
		{
			_platformProvider.ConvertRenderCall(brc, rc);
			return true;
		}
		return false;
	}

	bool PopulateRenderCallWithPen(IResource* res, PenRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEPENDENT_RES_RENDERCALL_IMPL1(Pen);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithPen(IResource* res, StrokeRenderCall_t& rc) const
	{
		PenRenderCall prc{};
		if (PopulateRenderCallWithPen(res, prc))
		{
			_platformProvider.ConvertRenderCall(prc, rc);
			return true;
		}
		return false;
	}

	bool PopulateRenderCallWithShadersGroup(IResource* res, MaterialRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEVICE_RES_RENDERCALL_IMPL1(ShadersGroup);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithShaderParameters(IResource* res, MaterialRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEPENDENT_RES_RENDERCALL_IMPL1(ShaderParameters);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithMaterial(IResource* res, MaterialRenderCall& rc) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		switch (resRef->GetType())
		{
			RM_POPULATE_DEPENDENT_RES_RENDERCALL_IMPL1(Material);
		default:
			ThrowAlways(L"Invalid Resource Type.");
		}
		return false;
	}

	bool PopulateRenderCallWithMaterial(IResource* res, Fill3DRenderCall_t& rc) const
	{
		MaterialRenderCall mrc{};
		if (PopulateRenderCallWithMaterial(res, mrc))
		{
			_platformProvider.ConvertRenderCall(mrc, rc);
			return true;
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

	decltype(auto) GetVertexBuffer(const Fill3DRenderCall_t& renderCall) const noexcept
	{
		return _fill3DVBMgr.GetBuffer(renderCall.VB);
	}

	decltype(auto) GetIndexBuffer(const Fill3DRenderCall_t& renderCall) const noexcept
	{
		return _fill3DIBMgr.GetBuffer(renderCall.IB);
	}

	decltype(auto) GetTextureBuffer(const BrushRenderCall& renderCall) const noexcept
	{
		return _textureBufferMgr.GetBuffer(renderCall.Texture);
	}

	decltype(auto) GetSamplerBuffer(const BrushRenderCall& renderCall) const noexcept
	{
		return _samplerBufferMgr.GetBuffer(renderCall.Sampler);
	}

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override
	{
		WRL::ComPtr<ResourceManager> me(this);
		auto dcl = std::make_shared<DrawCallList>(_deviceContext, me, rcb);
		dcl->Initialize();
		return dcl;
	}

	virtual bool TryGetCamera(IResource* res, Camera& camera) const
	{
		auto resRef = static_cast<ResourceRef*>(res);
		ThrowIfNot(resRef->GetType() == RT_Camera, L"Invalid Resource Type.");
		return _trcCamera.PopulateCamera(resRef->GetHandle(), camera);
	}

	virtual void UpdateGPU() override
	{
		ResourceManagerBase::UpdateGPU();

		_strokeVBMgr.Upload();
		_strokeIBMgr.Upload();
		_fillVBMgr.Upload();
		_fillIBMgr.Upload();
		_fill3DVBMgr.Upload();
		_fill3DIBMgr.Upload();

		_textureBufferMgr.Upload();
		_samplerBufferMgr.Upload();
	}
protected:
	RM_DECL_GET_GEOMETRY_TRC(LineGeometry);
	RM_DECL_GET_GEOMETRY_TRC(RectangleGeometry);
	RM_DECL_GET_GEOMETRY_TRC(PathGeometry);

	RM_DECL_GET_GEOMETRY3D_TRC(BoxGeometry3D);
	RM_DECL_GET_GEOMETRY3D_TRC(MeshGeometry3D);

	RM_DECL_GET_DEVICE_TRC(SolidColorTexture);
	RM_DECL_GET_DEVICE_TRC(Sampler);
	RM_DECL_GET_DEVICE_TRC(ShadersGroup);
	RM_DECL_GET_DEVICE_TRC(ShaderParameters);

	RM_DECL_GET_DEVICE_TRC(Brush);
	RM_DECL_GET_DEVICE_TRC(Pen);
	RM_DECL_GET_DEVICE_TRC(Material);

	RM_DECL_GET_DEVICE_TRC(Camera);

	DeviceContext _deviceContext;
private:
	PlatformProvider_t _platformProvider;

	VertexBufferManager _strokeVBMgr;
	IndexBufferManager _strokeIBMgr;
	VertexBufferManager _fillVBMgr;
	IndexBufferManager _fillIBMgr;
	VertexBufferManager _fill3DVBMgr;
	IndexBufferManager _fill3DIBMgr;

	TextureBufferManager _textureBufferMgr;
	SamplerBufferManager _samplerBufferMgr;
	ShaderBufferManager _shaderBufferMgr;

	RM_DECL_GEOMETRY_TRC_MEMBER(LineGeometry);
	RM_DECL_GEOMETRY_TRC_MEMBER(RectangleGeometry);
	RM_DECL_GEOMETRY_TRC_MEMBER(PathGeometry);

	RM_DECL_GEOMETRY3D_TRC_MEMBER(BoxGeometry3D);
	RM_DECL_GEOMETRY3D_TRC_MEMBER(MeshGeometry3D);

	RM_DECL_TEXTURE_TRC_MEMBER(SolidColorTexture);
	RM_DECL_SAMPLER_TRC_MEMBER(Sampler);
	RM_DECL_SHADERSGROUP_TRC_MEMBER(ShadersGroup);
	RM_DECL_SHADERPARAMETERS_TRC_MEMBER(ShaderParameters);

	RM_DECL_DEPENDENT_TRC_MEMBER(Brush);
	RM_DECL_DEPENDENT_TRC_MEMBER(Pen);
	RM_DECL_DEPENDENT_TRC_MEMBER(Material);

	RM_DECL_NO_DEPENDENT_TRC_MEMBER(Camera);
};

END_NS_PLATFORM