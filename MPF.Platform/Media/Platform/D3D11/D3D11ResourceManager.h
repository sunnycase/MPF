//
// MPF Platform
// D3D11 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#pragma once
#include <unordered_map>
#include "../D3D11PlatformProvider.h"
#include "../../ResourceManager.h"
#include <d3d11.h>

DEFINE_NS_PLATFORM_D3D11
class D3D11ResourceManager : public ResourceManager<PlatformId::D3D11>
{
public:
	D3D11ResourceManager(WRL::ComPtr<D3D11DeviceContext>& deviceContext, SwapChainUpdateContext& updateContext);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;
private:
	SwapChainUpdateContext& _updateContext;
};

END_NS_PLATFORM_D3D11