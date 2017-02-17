//
// MPF Platform
// Direct3D 11 Device Resource Transforms
// 作者：SunnyCase
// 创建时间：2017-02-16
//
#include "stdafx.h"
#include "../D3D11PlatformProvider.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace DirectX;
using namespace NS_PLATFORM_D3D11;

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<typename BufferProvider<BufferTypes::TextureBuffer>::RentUpdateContext>& textures, SolidColorTexture&& data)
{
	float color[4] = { data.color.R, data.color.G, data.color.B, data.color.A };

	BufferProvider<BufferTypes::TextureBuffer>::RentUpdateContext c{};
	for (size_t x = 0; x < 4; x++)
		for (size_t y = 0; y < 4; y++)
		{
			const auto beg = reinterpret_cast<byte*>(color);
			const auto end = reinterpret_cast<byte*>(color + 4);
			for (auto it = beg; it != end; ++it)
				c.Data.emplace_back(*it);
		}
	c.Desc.Tex2D = CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_R32G32B32A32_FLOAT, 4, 4);
	c.Dimension = 2;
	c.SrcRowPitch = 4 * sizeof(color);
	c.SRVDesc[0] = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32G32B32A32_FLOAT);
	textures.emplace_back(std::move(c));
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<typename BufferProvider<BufferTypes::SamplerBuffer>::RentUpdateContext>& samplers, Sampler&& data)
{
	BufferProvider<BufferTypes::SamplerBuffer>::RentUpdateContext c;
	c.Desc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	samplers.emplace_back(std::move(c));
}

void PlatformProvider<PlatformId::D3D11>::Transform(std::vector<typename BufferProvider<BufferTypes::ShaderBuffer>::RentUpdateContext>& shaders, ShadersGroup&& data)
{
	BufferProvider<BufferTypes::ShaderBuffer>::RentUpdateContext c;
	c.VertexShader = std::move(data.VertexShader);
	c.PixelShader = std::move(data.PixelShader);
	shaders.emplace_back(std::move(c));
}