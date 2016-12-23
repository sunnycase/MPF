//
// MPF Platform
// D3D9 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-07-23
//
#include "stdafx.h"
#include "D3D9ResourceManager.h"
#include "../../ResourceRef.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D9;
using namespace DirectX;

#define CTOR_IMPL1(T) _trc##T##(_strokeVBMgr)

D3D9ResourceManager::D3D9ResourceManager(IDirect3DDevice9* device)
	:ResourceManager([](D3D9VertexBufferManager& vbMgr) { vbMgr.Upload(); }, std::make_tuple(device, sizeof(StrokeVertex)), std::make_tuple(device, sizeof(StrokeVertex))),
	_device(device)
{
}

namespace
{
	class DrawCallList : public IDrawCallList
	{
		struct MyStrokeDrawCall : public StorkeRenderCall<RenderCall>
		{
			DirectX::XMFLOAT4X4 Transform;
		};
	public:
		DrawCallList(IDirect3DDevice9* device, D3D9ResourceManager* resMgr, RenderCommandBuffer* rcb)
			:_device(device), _resMgr(resMgr), _rcb(rcb)
		{

		}

		// 通过 IDrawCallList 继承
		virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) override
		{
			using namespace D3D9;

			ThrowIfFailed(_device->SetVertexShaderConstantF(VSCSlot_ModelTransform, modelTransform.m[0], VSCSize_ModelTransform));
			float constants[4] = { 0 };
			for (auto&& rc : _strokeRenderCalls)
			{
				auto vb = _resMgr->GetVertexBuffer(rc);
				ThrowIfFailed(_device->SetStreamSource(0, vb, 0, rc.Stride));
				constants[0] = rc.Thickness;
				ThrowIfFailed(_device->SetVertexShaderConstantF(VSCSlot_Thickness, constants, VSCSize_Thickness));
				ThrowIfFailed(_device->SetVertexShaderConstantF(VSCSlot_Color, rc.Color, VSCSize_Color));
				ThrowIfFailed(_device->SetVertexShaderConstantF(VSCSlot_GeometryTransform, rc.Transform.m[0], VSCSize_GeometryTransform));
				ThrowIfFailed(_device->DrawPrimitive(D3DPT_TRIANGLELIST, rc.StartVertex, rc.PrimitiveCount));
			}
		}

		virtual void Update() override
		{
			Update(false);
		}

		void Initialize()
		{
			Update(true);
		}
	private:
		// 通过 IDrawCallList 继承
		void PushGeometryDrawCall(IResource* resource, IResource* pen, const DirectX::XMFLOAT4X4 transform)
		{
			if (pen)
			{
				MyStrokeDrawCall rc;
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
						rc.Transform = transform;
						_strokeRenderCalls.emplace_back(rc);
					}
					else
					{
						assert(false && "Geometry not found.");
					}
				}
			}
		}

		void Update(bool addResDependent)
		{
			_strokeRenderCalls.clear();
			for (auto&& geoRef : _rcb->GetGeometries())
			{
				PushGeometryDrawCall(geoRef.Geometry.Get(), geoRef.Pen.Get(), geoRef.Transform);
				if (addResDependent)
				{
					auto me = shared_from_this();
					_resMgr->AddDependentDrawCallList(me, geoRef.Geometry.Get());
					_resMgr->AddDependentDrawCallList(me, geoRef.Pen.Get());
				}
			}
		}
	private:
		ComPtr<IDirect3DDevice9> _device;
		ComPtr<D3D9ResourceManager> _resMgr;
		ComPtr<RenderCommandBuffer> _rcb;
		std::vector<MyStrokeDrawCall> _strokeRenderCalls;
	};
}

std::shared_ptr<IDrawCallList> D3D9ResourceManager::CreateDrawCallList(RenderCommandBuffer* rcb)
{
	auto ret = std::make_shared<DrawCallList>(_device.Get(), this, rcb);
	ret->Initialize();
	return ret;
}

void D3D9ResourceManager::BeginResetDevice()
{
	_strokeVBMgr.BeginResetDevice();
}

void D3D9ResourceManager::EndResetDevice()
{
	_strokeVBMgr.EndResetDevice();
}
