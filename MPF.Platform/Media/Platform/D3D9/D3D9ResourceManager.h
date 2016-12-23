//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#pragma once
#include <d3d9.h>
#include "D3D9BufferManager.h"
#include <unordered_map>
#include "../../ResourceManager.h"

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class D3D9ResourceManager : public ResourceManager<PlatformId::D3D9>
{
public:
	D3D9ResourceManager(IDirect3DDevice9* device);

	virtual std::shared_ptr<IDrawCallList> CreateDrawCallList(RenderCommandBuffer* rcb) override;

	virtual void BeginResetDevice() override;
	virtual void EndResetDevice() override;
private:
	WRL::ComPtr<IDirect3DDevice9> _device;
};

END_NS_PLATFORM