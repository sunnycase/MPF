//
// MPF Platform
// Transformed Resource Container
// 作者：SunnyCase
// 创建时间：2016-12-23
//
#pragma once
#include "../../inc/common.h"
#include <set>
#include <optional>
#include <vector>
#include <unordered_map>
#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>
#include <tuple>
#include "ResourceContainer.h"
#include "ResourceManagerBase.h"
#include "Platform/PlatformProvider.h"
#include "Platform/PlatformProviderTraits.h"
#include "Geometry.h"
#include "Brush.h"
#include "Material.h"

DEFINE_NS_PLATFORM

namespace Details
{
	template<typename TDerived, typename TData, typename TRentInfo>
	class TransformedResourceContainerImpl : public ITransformedResourceContainer<TData>
	{
	public:
		TransformedResourceContainerImpl(RenderCallAwareness rcAware)
			:_rcAware(rcAware)
		{

		}

		virtual void Update(UINT_PTR handle, TData&& data) override
		{
			_itemsToTrans.push({ handle, std::move(data) });
		}

		virtual void Remove(UINT_PTR handle) override
		{
			_itemsToRemove.push(handle);
		}

		virtual void UpdateDeviceResources(ResourceManagerBase& resMgr, std::function<void(UINT_PTR, RenderCallAwareness)>& registerRenderCallUpdate)
		{
			{
				UINT_PTR toRemove;
				std::vector<UINT_PTR> nextTimeRemove;
				while (_itemsToRemove.try_pop(toRemove))
				{
					auto it = _rentInfos.find(toRemove);
					if (it != _rentInfos.end())
					{
						GetThis()->RetireRent(it->second);
						_rentInfos.erase(it);
					}
					else
						nextTimeRemove.emplace_back(toRemove);
				}
				for (auto&& handle : nextTimeRemove)
					_itemsToRemove.push(handle);
			}
			{
				static std::pair<UINT_PTR, TData> toTrans;
				bool affectRenderCall;
				while (_itemsToTrans.try_pop(toTrans))
				{
					auto oldRentIt = _rentInfos.find(toTrans.first);
					if (oldRentIt != _rentInfos.end())
						oldRentIt->second = GetThis()->Transform(oldRentIt->second, std::move(toTrans.second), affectRenderCall);
					else
						_rentInfos.emplace(toTrans.first, GetThis()->Transform({}, std::move(toTrans.second), affectRenderCall));
					
					registerRenderCallUpdate(toTrans.first, _rcAware);
				}
			}
		}

		bool TryGetRentInfo(UINT_PTR handle, TRentInfo& rentInfo) const
		{
			auto rentIt = _rentInfos.find(handle);
			if (rentIt != _rentInfos.end())
			{
				rentInfo = rentIt->second;
				return true;
			}
			return false;
		}
	private:
		TDerived* GetThis() noexcept { return static_cast<TDerived*>(this); }
		TDerived const* GetThis() const noexcept { return static_cast<TDerived const*>(this); }
	private:
		concurrency::concurrent_queue<std::pair<UINT_PTR, TData>> _itemsToTrans;
		concurrency::concurrent_queue<UINT_PTR> _itemsToRemove;
		std::unordered_map<UINT_PTR, TRentInfo> _rentInfos;
		const RenderCallAwareness _rcAware;
	};

	template<typename TDerived, typename TData>
	class DependentResourceContainerImpl : public ITransformedResourceContainer<TData>
	{
	public:
		DependentResourceContainerImpl(RenderCallAwareness rcAware)
			:_rcAware(rcAware)
		{

		}

		virtual void Update(UINT_PTR handle, TData&& data) override
		{
			_itemsToTrans.push({ handle, std::move(data) });
		}

		virtual void Remove(UINT_PTR handle) override
		{
			_itemsToRemove.push(handle);
		}

		virtual void UpdateDeviceResources(ResourceManagerBase& resMgr, std::function<void(UINT_PTR, RenderCallAwareness)>& registerRenderCallUpdate)
		{
			{
				UINT_PTR toRemove;
				std::vector<UINT_PTR> nextTimeRemove;
				while (_itemsToRemove.try_pop(toRemove))
				{
					auto it = _rentInfos.find(toRemove);
					if (it != _rentInfos.end())
					{
						_data.RetireAndCleanupResource(it->second);
						_rentInfos.erase(it);
					}
					else
						nextTimeRemove.emplace_back(toRemove);
				}
				for (auto&& handle : nextTimeRemove)
					_itemsToRemove.push(handle);
			}
			{
				static std::pair<UINT_PTR, TData> toTrans;
				while (_itemsToTrans.try_pop(toTrans))
				{
					auto oldRentIt = _rentInfos.find(toTrans.first);
					if (oldRentIt != _rentInfos.end())
					{
						auto& oldRes = _data.FindResource(oldRentIt->second);
						GetThis()->UnregisterDependentResource(resMgr, toTrans.first, oldRes);
						oldRes = std::move(toTrans.second);
						GetThis()->RegisterDependentResource(resMgr, toTrans.first, oldRes);
					}
					else
					{
						auto handle = _data.Allocate();
						auto& oldRes = _data.FindResource(handle);
						oldRes = std::move(toTrans.second);
						_rentInfos.emplace(toTrans.first, handle);
						GetThis()->RegisterDependentResource(resMgr, toTrans.first, oldRes);
					}
					registerRenderCallUpdate(toTrans.first, _rcAware);
				}
			}
		}

		bool TryGetRentInfo(UINT_PTR handle, UINT_PTR& rentInfo) const
		{
			auto rentIt = _rentInfos.find(handle);
			if (rentIt != _rentInfos.end())
			{
				rentInfo = rentIt->second;
				return true;
			}
			return false;
		}

		const TData& GetResource(UINT_PTR rentInfo) const
		{
			return _data.FindResource(rentInfo);
		}
	private:
		TDerived* GetThis() noexcept { return static_cast<TDerived*>(this); }
		TDerived const* GetThis() const noexcept { return static_cast<TDerived const*>(this); }
	private:
		concurrency::concurrent_queue<std::pair<UINT_PTR, TData>> _itemsToTrans;
		concurrency::concurrent_queue<UINT_PTR> _itemsToRemove;
		ResourceContainer<TData> _data;
		std::unordered_map<UINT_PTR, UINT_PTR> _rentInfos;
		const RenderCallAwareness _rcAware;
	};
}

template<PlatformId PId, typename TData, typename TVertex, typename TRenderCall>
class TransformedGeometryResourceContainer : public Details::TransformedResourceContainerImpl<TransformedGeometryResourceContainer<PId, TData, TVertex, TRenderCall>, TData, GeometryRentInfo>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using VertexBufferManager = typename PlatformProviderTraits<PId>::VertexBufferManager;
	using IndexBufferManager = typename PlatformProviderTraits<PId>::IndexBufferManager;

	TransformedGeometryResourceContainer(RenderCallAwareness rcAware, VertexBufferManager& vbMgr, IndexBufferManager& ibMgr)
		:TransformedResourceContainerImpl(rcAware), _vbMgr(vbMgr), _ibMgr(ibMgr)
	{

	}

	bool PopulateRenderCall(UINT_PTR handle, TRenderCall& rc) const
	{
		GeometryRentInfo rent;
		if (TryGetRentInfo(handle, rent))
		{
			_vbMgr.GetRenderCall(rent.Vertices, rc);
			_ibMgr.GetRenderCall(rent.Indices, rc);
			return true;
		}
		return false;
	}
private:
	friend class Details::TransformedResourceContainerImpl<TransformedGeometryResourceContainer, TData, GeometryRentInfo>;

	void RetireRent(const GeometryRentInfo& rent)
	{
		_vbMgr.Retire(rent.Vertices);
		_ibMgr.Retire(rent.Indices);
	}

	GeometryRentInfo Transform(const std::optional<GeometryRentInfo>& oldRent, TData&& data, bool& affectRenderCall)
	{
		static std::vector<TVertex> vertices;
		static std::vector<size_t> indices;

		vertices.clear();
		indices.clear();
		_platformProvider.Transform(vertices, indices, std::move(data));

		GeometryRentInfo newRent;
		bool needAllocV = false, needAllocI = false;
		if (!oldRent.has_value())
			needAllocV = needAllocI = true;
		else
		{
			if (oldRent.value().Vertices.length != vertices.size())
			{
				_vbMgr.Retire(oldRent.value().Vertices);
				needAllocV = true;
			}
			if (oldRent.value().Indices.length != indices.size())
			{
				_ibMgr.Retire(oldRent.value().Indices);
				needAllocI = true;
			}
		}
		if (needAllocV)
			newRent.Vertices = _vbMgr.Allocate(vertices.size());
		if (needAllocI)
			newRent.Indices = _ibMgr.Allocate(indices.size());
		_vbMgr.Update(newRent.Vertices, 0, vertices.data(), vertices.size());
		_ibMgr.Update(newRent.Indices, 0, indices.data(), indices.size());
		affectRenderCall = needAllocV || needAllocI;
		return newRent;
	}
private:
	PlatformProvider<PId> _platformProvider;
	VertexBufferManager& _vbMgr;
	IndexBufferManager& _ibMgr;
};

template<PlatformId PId, typename TData, typename TRenderCall>
class TransformedTextureResourceContainer : public Details::TransformedResourceContainerImpl<TransformedTextureResourceContainer<PId, TData, TRenderCall>, TData, TextureRentInfo>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using TextureBufferManager = typename PlatformProviderTraits<PId>::TextureBufferManager;
	using TextureBufferRentUpdateContext = typename TextureBufferManager::RentUpdateContext;

	TransformedTextureResourceContainer(RenderCallAwareness rcAware, TextureBufferManager& tbMgr)
		:TransformedResourceContainerImpl(rcAware), _tbMgr(tbMgr)
	{

	}

	bool PopulateRenderCall(UINT_PTR handle, TRenderCall& rc) const
	{
		TextureRentInfo rent;
		if (TryGetRentInfo(handle, rent))
		{
			_tbMgr.GetRenderCall(rent.Textures, rc);
			return true;
		}
		return false;
	}
private:
	friend class Details::TransformedResourceContainerImpl<TransformedTextureResourceContainer, TData, TextureRentInfo>;

	void RetireRent(const TextureRentInfo& rent)
	{
		_tbMgr.Retire(rent.Textures);
	}

	TextureRentInfo Transform(const std::optional<TextureRentInfo>& oldRent, TData&& data, bool& affectRenderCall)
	{
		std::vector<TextureBufferRentUpdateContext> textures;

		_platformProvider.Transform(textures, std::move(data));

		TextureRentInfo newRent;
		bool needAllocT = false;
		if (!oldRent.has_value())
			needAllocT = true;
		else
		{
			if (oldRent.value().Textures.length != textures.size())
			{
				_tbMgr.Retire(oldRent.value().Textures);
				needAllocT = true;
			}
		}
		if (needAllocT)
			newRent.Textures = _tbMgr.Allocate(textures.size());
		_tbMgr.Update(newRent.Textures, 0, std::move(textures));
		affectRenderCall = needAllocT;
		return newRent;
	}
private:
	PlatformProvider<PId> _platformProvider;
	TextureBufferManager& _tbMgr;
};

template<PlatformId PId, typename TData, typename TRenderCall>
class TransformedSamplerResourceContainer : public Details::TransformedResourceContainerImpl<TransformedSamplerResourceContainer<PId, TData, TRenderCall>, TData, SamplerRentInfo>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using SamplerBufferManager = typename PlatformProviderTraits<PId>::SamplerBufferManager;
	using SamplerBufferRentUpdateContext = typename SamplerBufferManager::RentUpdateContext;

	TransformedSamplerResourceContainer(RenderCallAwareness rcAware, SamplerBufferManager& sbMgr)
		:TransformedResourceContainerImpl(rcAware), _sbMgr(sbMgr)
	{

	}

	bool PopulateRenderCall(UINT_PTR handle, TRenderCall& rc) const
	{
		SamplerRentInfo rent;
		if (TryGetRentInfo(handle, rent))
		{
			_sbMgr.GetRenderCall(rent.Samplers, rc);
			return true;
		}
		return false;
	}
private:
	friend class Details::TransformedResourceContainerImpl<TransformedSamplerResourceContainer, TData, SamplerRentInfo>;

	void RetireRent(const SamplerRentInfo& rent)
	{
		_sbMgr.Retire(rent.Samplers);
	}

	SamplerRentInfo Transform(const std::optional<SamplerRentInfo>& oldRent, TData&& data, bool& affectRenderCall)
	{
		std::vector<SamplerBufferRentUpdateContext> samplers;

		_platformProvider.Transform(samplers, std::move(data));

		SamplerRentInfo newRent;
		bool needAllocS = false;
		if (!oldRent.has_value())
			needAllocS = true;
		else
		{
			if (oldRent.value().Samplers.length != samplers.size())
			{
				_sbMgr.Retire(oldRent.value().Samplers);
				needAllocS = true;
			}
		}
		if (needAllocS)
			newRent.Samplers = _sbMgr.Allocate(samplers.size());
		_sbMgr.Update(newRent.Samplers, 0, std::move(samplers));
		affectRenderCall = needAllocS;
		return newRent;
	}
private:
	PlatformProvider<PId> _platformProvider;
	SamplerBufferManager& _sbMgr;
};

template<PlatformId PId, typename TRenderCall>
class TransformedBrushResourceContainer : public Details::DependentResourceContainerImpl<TransformedBrushResourceContainer<PId, TRenderCall>, Brush>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;

	TransformedBrushResourceContainer(RenderCallAwareness rcAware)
		:DependentResourceContainerImpl(rcAware)
	{

	}

	template<typename TResourceManager>
	bool PopulateRenderCall(const TResourceManager& resMgr, UINT_PTR handle, TRenderCall& rc) const
	{
		UINT_PTR rent;
		if (TryGetRentInfo(handle, rent))
		{
			bool success = true;
			auto& data = GetResource(rent);
			if(data.Texture)
				success = success && resMgr.PopulateRenderCallWithTexture(data.Texture.Get(), rc);
			if (data.Sampler)
				success = success && resMgr.PopulateRenderCallWithSampler(data.Sampler.Get(), rc);
			return success;
		}
		return false;
	}
private:
	friend class Details::DependentResourceContainerImpl<TransformedBrushResourceContainer, Brush>;

	void RegisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Brush& data)
	{
		if (data.Texture)
			resMgr.AddDependentResource(handle, data.Texture.Get());
		if (data.Sampler)
			resMgr.AddDependentResource(handle, data.Sampler.Get());
	}

	void UnregisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Brush& data)
	{

	}
private:
	PlatformProvider<PId> _platformProvider;
};

template<PlatformId PId, typename TRenderCall>
class TransformedPenResourceContainer : public Details::DependentResourceContainerImpl<TransformedPenResourceContainer<PId, TRenderCall>, Pen>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;

	TransformedPenResourceContainer(RenderCallAwareness rcAware)
		:DependentResourceContainerImpl(rcAware)
	{

	}

	template<typename TResourceManager>
	bool PopulateRenderCall(const TResourceManager& resMgr, UINT_PTR handle, TRenderCall& rc) const
	{
		UINT_PTR rent;
		if (TryGetRentInfo(handle, rent))
		{
			bool success = true;
			auto& data = GetResource(rent);
			if (data.Brush)
				success = success && resMgr.PopulateRenderCallWithBrush(data.Brush.Get(), rc.Brush);
			rc.Thickness = data.Thickness;
			return success;
		}
		return false;
	}
private:
	friend class Details::DependentResourceContainerImpl<TransformedPenResourceContainer, Pen>;

	void RegisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Pen& data)
	{
		if (data.Brush)
			resMgr.AddDependentResource(handle, data.Brush.Get());
	}

	void UnregisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Pen& data)
	{

	}
private:
	PlatformProvider<PId> _platformProvider;
};

template<PlatformId PId>
class TransformedCameraResourceContainer : public Details::DependentResourceContainerImpl<TransformedCameraResourceContainer<PId>, Camera>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;

	TransformedCameraResourceContainer(RenderCallAwareness rcAware)
		:DependentResourceContainerImpl(rcAware)
	{

	}

	bool PopulateCamera(UINT_PTR handle, Camera& camera) const
	{
		UINT_PTR rent;
		if (TryGetRentInfo(handle, rent))
		{
			camera = GetResource(rent);
			return true;
		}
		return false;
	}
private:
	friend class Details::DependentResourceContainerImpl<TransformedCameraResourceContainer, Camera>;

	void RegisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Camera& data)
	{

	}

	void UnregisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Camera& data)
	{

	}
private:
	PlatformProvider<PId> _platformProvider;
};

template<PlatformId PId, typename TData, typename TRenderCall>
class TransformedShadersGroupResourceContainer : public Details::TransformedResourceContainerImpl<TransformedShadersGroupResourceContainer<PId, TData, TRenderCall>, TData, ShadersGroupRentInfo>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;
	using ShaderBufferManager = typename PlatformProviderTraits<PId>::ShaderBufferManager;
	using ShaderBufferRentUpdateContext = typename ShaderBufferManager::RentUpdateContext;

	TransformedShadersGroupResourceContainer(RenderCallAwareness rcAware, ShaderBufferManager& sbMgr)
		:TransformedResourceContainerImpl(rcAware), _sbMgr(sbMgr)
	{

	}

	bool PopulateRenderCall(UINT_PTR handle, TRenderCall& rc) const
	{
		ShadersGroupRentInfo rent;
		if (TryGetRentInfo(handle, rent))
		{
			_sbMgr.GetRenderCall(rent.Shaders, rc);
			return true;
		}
		return false;
	}
private:
	friend class Details::TransformedResourceContainerImpl<TransformedShadersGroupResourceContainer, TData, ShadersGroupRentInfo>;

	void RetireRent(const ShadersGroupRentInfo& rent)
	{
		_sbMgr.Retire(rent.Shaders);
	}

	ShadersGroupRentInfo Transform(const std::optional<ShadersGroupRentInfo>& oldRent, TData&& data, bool& affectRenderCall)
	{
		std::vector<ShaderBufferRentUpdateContext> shaders;

		_platformProvider.Transform(shaders, std::move(data));

		ShadersGroupRentInfo newRent;
		bool needAllocS = false;
		if (!oldRent.has_value())
			needAllocS = true;
		else
		{
			if (oldRent.value().Shaders.length != shaders.size())
			{
				_sbMgr.Retire(oldRent.value().Shaders);
				needAllocS = true;
			}
		}
		if (needAllocS)
			newRent.Shaders = _sbMgr.Allocate(shaders.size());
		_sbMgr.Update(newRent.Shaders, 0, std::move(shaders));
		affectRenderCall = needAllocS;
		return newRent;
	}
private:
	PlatformProvider<PId> _platformProvider;
	ShaderBufferManager& _sbMgr;
};

template<PlatformId PId, typename TRenderCall>
class TransformedShaderParametersResourceContainer : public Details::DependentResourceContainerImpl<TransformedShaderParametersResourceContainer<PId, TRenderCall>, ShaderParameters>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;

	TransformedShaderParametersResourceContainer(RenderCallAwareness rcAware)
		:DependentResourceContainerImpl(rcAware)
	{

	}

	template<typename TResourceManager>
	bool PopulateRenderCall(const TResourceManager& resMgr, UINT_PTR handle, TRenderCall& rc) const
	{
		UINT_PTR rent;
		if (TryGetRentInfo(handle, rent))
		{
			bool success = true;
			auto& data = GetResource(rent);

			size_t bid = 0;
			for (auto& brush : data.Brushes)
			{
				if (brush)
					success = success && resMgr.PopulateRenderCallWithBrush(brush.Get(), rc.ShaderParameters.Brushes[bid++]);
			}
			rc.ShaderParameters.Variables = &data.Variables;
			return success;
		}
		return false;
	}
private:
	friend class Details::DependentResourceContainerImpl<TransformedShaderParametersResourceContainer, ShaderParameters>;

	void RegisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const ShaderParameters& data)
	{
		for (auto&& brush : data.Brushes)
		{
			if (brush)
				resMgr.AddDependentResource(handle, brush.Get());
		}
	}

	void UnregisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const ShaderParameters& data)
	{

	}
private:
	PlatformProvider<PId> _platformProvider;
};

template<PlatformId PId, typename TRenderCall>
class TransformedMaterialResourceContainer : public Details::DependentResourceContainerImpl<TransformedMaterialResourceContainer<PId, TRenderCall>, Material>
{
public:
	using PlatformProvider_t = PlatformProvider<PId>;

	TransformedMaterialResourceContainer(RenderCallAwareness rcAware)
		:DependentResourceContainerImpl(rcAware)
	{

	}

	template<typename TResourceManager>
	bool PopulateRenderCall(const TResourceManager& resMgr, UINT_PTR handle, TRenderCall& rc) const
	{
		UINT_PTR rent;
		if (TryGetRentInfo(handle, rent))
		{
			bool success = true;
			auto& data = GetResource(rent);
			if (data.Shader)
				success = success && resMgr.PopulateRenderCallWithShadersGroup(data.Shader.Get(), rc);
			if (data.ShaderParameters)
				success = success && resMgr.PopulateRenderCallWithShaderParameters(data.ShaderParameters.Get(), rc);
			return success;
		}
		return false;
	}
private:
	friend class Details::DependentResourceContainerImpl<TransformedMaterialResourceContainer, Material>;

	void RegisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Material& data)
	{
		if (data.Shader)
			resMgr.AddDependentResource(handle, data.Shader.Get());
		if (data.ShaderParameters)
			resMgr.AddDependentResource(handle, data.ShaderParameters.Get());
	}

	void UnregisterDependentResource(ResourceManagerBase& resMgr, UINT_PTR handle, const Material& data)
	{

	}
private:
	PlatformProvider<PId> _platformProvider;
};

END_NS_PLATFORM