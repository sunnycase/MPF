//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#pragma once
#include <unordered_map>
#include <memory>
#include "../../ResourceManager.h"
#include "../D3D9PlatformProvider.h"
#include <d3d9.h>

DEFINE_NS_PLATFORM_D3D9

class D3D9ResourceManager : public ResourceManager<PlatformId::D3D9>
{
public:
	D3D9ResourceManager(WRL::ComPtr<D3D9DeviceContext>& deviceContext);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;

	virtual void BeginResetDevice() override;
	virtual void EndResetDevice() override;
private:
};

END_NS_PLATFORM_D3D9