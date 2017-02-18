//
// MPF Platform
// Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-21
//
#include "stdafx.h"
#include "ResourceManagerBase.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"
#include "FontManager.h"
using namespace WRL;
using namespace NS_PLATFORM;

#define CTOR_IMPL1(T) \
_container##T(std::make_shared<ResourceContainer<T>>())

ResourceManagerBase::ResourceManagerBase()
	:_containerCS(1000), _fontManager(std::make_shared<FontManager>())
{
}						   

HRESULT ResourceManagerBase::CreateResource(ResourceType resType, IResource ** res)
{
	try
	{
		*res = Make<ResourceRef>(GetWeakContext(), resType, _nextResourceId++).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

#define UPDATE_GEOMETRY_RES_IMPL(T, src)								\
try																		\
{																		\
	auto handle = static_cast<ResourceRef*>(res)->GetHandle();			\
	assert(static_cast<ResourceRef*>(res)->GetType() == RT_##T);		\
	Get##T##StrokeTRC().Update(handle, src);							\
	Get##T##FillTRC().Update(handle, src);								\
	return S_OK;														\
}																		\
CATCH_ALL()

HRESULT ResourceManagerBase::UpdateLineGeometry(IResource * res, LineGeometryData * data)
{
	UPDATE_GEOMETRY_RES_IMPL(LineGeometry, { *data });
}

HRESULT ResourceManagerBase::UpdateRectangleGeometry(IResource * res, RectangleGeometryData * data)
{
	UPDATE_GEOMETRY_RES_IMPL(RectangleGeometry, { *data });
}

namespace
{
	template<class T>
	T* Read(byte*& data)
	{
		auto old = reinterpret_cast<T*>(data);
		data += sizeof(T);
		return reinterpret_cast<T*>(old);
	}

#define UPDATE_PATH_GEO_IMPL1(T)											\
case T:																		\
	seg.Operation = T;														\
	seg.Data.##T## = *Read<Segment::tagData::tag##T>(data);					\
	break;
}

STDMETHODIMP ResourceManagerBase::UpdatePathGeometry(IResource * res, byte * data, UINT32 length)
{
	using namespace PathGeometrySegments;

	std::vector<Segment> segments;
	const auto end = data + length;
	while (data < end)
	{
		Segment seg;
		const auto type = Read<Operations>(data);
		switch (*type)
		{
			UPDATE_PATH_GEO_IMPL1(MoveTo);
			UPDATE_PATH_GEO_IMPL1(LineTo);
			UPDATE_PATH_GEO_IMPL1(ArcTo);
		default:
			continue;
		}
		segments.emplace_back(std::move(seg));
	}
	UPDATE_GEOMETRY_RES_IMPL(PathGeometry, { segments });
}

HRESULT ResourceManagerBase::UpdatePathGeometry(IResource * res, std::vector<PathGeometrySegments::Segment>&& segments) noexcept
{
	UPDATE_GEOMETRY_RES_IMPL(PathGeometry, { segments });
}

#define UPDATE_DEVICE_RES_IMPL(T, src)									\
try																		\
{																		\
	auto handle = static_cast<ResourceRef*>(res)->GetHandle();			\
	assert(static_cast<ResourceRef*>(res)->GetType() == RT_##T);		\
	Get##T##TRC().Update(handle, src);									\
	return S_OK;														\
}																		\
CATCH_ALL()

HRESULT ResourceManagerBase::UpdateBrush(IResource * res, IResource* texture, IResource* sampler)
{
	UPDATE_DEVICE_RES_IMPL(Brush, (Brush{ texture, sampler }));
}

HRESULT ResourceManagerBase::UpdatePen(IResource * res, float thickness, IResource * brush)
{
	UPDATE_DEVICE_RES_IMPL(Pen, (Pen{ thickness, brush }));
}

STDMETHODIMP ResourceManagerBase::UpdateCamera(IResource * res, float * viewMatrix, float * projectionMatrix)
{
	Camera resObj{};
	XMStoreFloat4x4(&resObj.View, XMMatrixTranspose(DirectX::XMMATRIX(viewMatrix)));
	XMStoreFloat4x4(&resObj.Projection, XMMatrixTranspose(DirectX::XMMATRIX(projectionMatrix)));

	UPDATE_DEVICE_RES_IMPL(Camera, std::move(resObj));
}

STDMETHODIMP ResourceManagerBase::UpdateShaderParameters(IResource * res, BYTE * data, UINT dataSize, IResource * brushes[], UINT brushesCount)
{
	ShaderParameters resObj;
	for (size_t i = 0; i < brushesCount; i++)
		resObj.Brushes.emplace_back(brushes[i]);
	resObj.Variables.assign(data, data + dataSize);
	UPDATE_DEVICE_RES_IMPL(ShaderParameters, std::move(resObj));
}

STDMETHODIMP ResourceManagerBase::UpdateMaterial(IResource * res, IResource * shader, IResource * shaderParameters)
{
	UPDATE_DEVICE_RES_IMPL(Material, (Material{ shader, shaderParameters }));
}

#define UPDATE_GEOMETRY3D_RES_IMPL(T, src)								\
try																		\
{																		\
	auto handle = static_cast<ResourceRef*>(res)->GetHandle();			\
	assert(static_cast<ResourceRef*>(res)->GetType() == RT_##T);		\
	Get##T##FillTRC().Update(handle, src);								\
	return S_OK;														\
}																		\
CATCH_ALL()

HRESULT ResourceManagerBase::UpdateBoxGeometry3D(IResource * res, BoxGeometry3DData* data)
{
	UPDATE_GEOMETRY3D_RES_IMPL(BoxGeometry3D, { *data });
}

HRESULT ResourceManagerBase::UpdateMeshGeometry3D(IResource * res, MeshGeometry3DData* data)
{
	std::vector<MeshGeometry3D::MeshVertex> vertices;
	std::vector<size_t> indices;

	const auto length = data->PositionsCount;
	vertices.resize(data->PositionsCount);
	for (size_t i = 0; i < length; i++)
	{
		auto& target = vertices[i];
		target.Position = data->Positions[i];
		if (i < data->NormalsCount)
			target.Normal = data->Normals[i];
		if (i < data->TextureCoordinatesCount)
			target.TexCoord = data->TextureCoordinates[i];
	}
	const auto indLength = data->IndicesCount;
	for (size_t i = 0; i < indLength; i++)
		indices.emplace_back(data->Indices[i]);

	UPDATE_GEOMETRY3D_RES_IMPL(MeshGeometry3D, (MeshGeometry3D{ std::move(vertices), std::move(indices) }));
}

HRESULT ResourceManagerBase::UpdateSolidColorTexture(IResource * res, ColorF * color)
{
	UPDATE_DEVICE_RES_IMPL(SolidColorTexture, { *color });
}

HRESULT ResourceManagerBase::UpdateSampler(IResource * res, SamplerData * data)
{
	UPDATE_DEVICE_RES_IMPL(Sampler, { *data });
}

HRESULT ResourceManagerBase::UpdateShadersGroup(IResource * res, ShadersGroupData * data)
{
	ShadersGroup src;
	src.VertexShader.assign(reinterpret_cast<byte*>(data->VertexShader),
		reinterpret_cast<byte*>(data->VertexShader) + data->VertexShaderLength);
	src.PixelShader.assign(reinterpret_cast<byte*>(data->PixelShader), 
		reinterpret_cast<byte*>(data->PixelShader) + data->PixelShaderLength);
	UPDATE_DEVICE_RES_IMPL(ShadersGroup, std::move(src));
}

HRESULT ResourceManagerBase::CreateRenderCommandBuffer(IRenderCommandBuffer ** buffer)
{
	try
	{
		*buffer = Make<RenderCommandBuffer>(this).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

#define ADDCL_IMPL1(T)															\
	case RT_##T:																\
		Get##T(resRef->GetHandle()).AddDependentDrawCallList(std::move(dcl));	\
		break;

void ResourceManagerBase::AddDependentDrawCallList(std::weak_ptr<IDrawCallList>&& dcl, IResource* res)
{
	auto locker = _containerCS.Lock();
	auto handle = static_cast<ResourceRef*>(res)->GetHandle();
	_dependentDrawCallLists.emplace(handle, std::move(dcl));
}

void ResourceManagerBase::AddDependentResource(UINT_PTR handle, IResource * res)
{
	auto locker = _containerCS.Lock();
	auto listen = static_cast<ResourceRef*>(res)->GetHandle();
	_dependentResources.emplace(listen, handle);
}

#define RETIRE_GEOMETRY_RES_IMPL(T)												\
	case RT_##T:																\
		Get##T##StrokeTRC().Remove(resRef->GetHandle());						\
		Get##T##FillTRC().Remove(resRef->GetHandle());							\
		break;

#define RETIRE_GEOMETRY3D_RES_IMPL(T)											\
	case RT_##T:																\
		Get##T##FillTRC().Remove(resRef->GetHandle());							\
		break;

void ResourceManagerBase::RetireResource(IResource * res)
{
	if (!res) return;
	auto resRef = static_cast<ResourceRef*>(res);
	switch (resRef->GetType())
	{
		RETIRE_GEOMETRY_RES_IMPL(LineGeometry);
		RETIRE_GEOMETRY_RES_IMPL(RectangleGeometry);
		RETIRE_GEOMETRY_RES_IMPL(PathGeometry);
		//RETIRERES_IMPL1(Pen);
		//RETIRERES_IMPL1(Camera);
		//RETIRERES_IMPL1(ShaderParameters);
		//RETIRERES_IMPL1(Material);

		RETIRE_GEOMETRY3D_RES_IMPL(BoxGeometry3D);
		RETIRE_GEOMETRY3D_RES_IMPL(MeshGeometry3D);
	default:
		break;
	}
}

#define UPDATE_GEOMTRY_TRC_DEVRES_IMPL(T)	\
Get##T##StrokeTRC().UpdateDeviceResources(*this, registerRenderCallUpdate);	\
Get##T##FillTRC().UpdateDeviceResources(*this, registerRenderCallUpdate)

#define UPDATE_GEOMTRY3D_TRC_DEVRES_IMPL(T)	\
Get##T##FillTRC().UpdateDeviceResources(*this, registerRenderCallUpdate)

#define UPDATE_DEVICE_TRC_DEVRES_IMPL(T)	\
Get##T##TRC().UpdateDeviceResources(*this, registerRenderCallUpdate)

void ResourceManagerBase::UpdateGPU()
{
	std::vector<UINT_PTR> updatedRes;
	std::function<void(UINT_PTR, RenderCallAwareness)> registerRenderCallUpdate = [&](UINT_PTR handle, RenderCallAwareness rcAware)
	{
		updatedRes.emplace_back(handle);

		auto range = _dependentResources.equal_range(handle);
		for (auto it = range.first; it != range.second; ++it)
			updatedRes.emplace_back(it->second);
	};

	UPDATE_GEOMTRY_TRC_DEVRES_IMPL(LineGeometry);
	UPDATE_GEOMTRY_TRC_DEVRES_IMPL(RectangleGeometry);
	UPDATE_GEOMTRY_TRC_DEVRES_IMPL(PathGeometry);

	UPDATE_GEOMTRY3D_TRC_DEVRES_IMPL(BoxGeometry3D);
	UPDATE_GEOMTRY3D_TRC_DEVRES_IMPL(MeshGeometry3D);

	UPDATE_DEVICE_TRC_DEVRES_IMPL(SolidColorTexture);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(Sampler);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(ShadersGroup);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(ShaderParameters);

	UPDATE_DEVICE_TRC_DEVRES_IMPL(Brush);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(Pen);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(Material);
	UPDATE_DEVICE_TRC_DEVRES_IMPL(Camera);

	auto locker = _containerCS.Lock();
	for (auto&& handle : updatedRes)
	{
		auto range = _dependentDrawCallLists.equal_range(handle);
		for (auto it = range.first; it != range.second;++it)
		{
			if (auto dcl = it->second.lock())
				dcl->Update();
			//else
			//	it = _dependentDrawCallLists.erase(it);
		}
	}
}

HRESULT ResourceManagerBase::CreateFontFaceFromMemory(INT_PTR buffer, UINT64 size, UINT faceIndex, IFontFace **fontFace)
{
	try
	{
		_fontManager->CreateFontFaceFromMemory(buffer, size, faceIndex, fontFace);
		return S_OK;
	}
	CATCH_ALL();
}