//
// MPF Platform
// D3D11 Resource Manager
// 作者：SunnyCase
// 创建时间：2016-08-28
//
#include "stdafx.h"
#include "D3D11ResourceManager.h"
#include "D3D11DeviceContext.h"
using namespace WRL;
using namespace NS_PLATFORM;
using namespace NS_PLATFORM_D3D11;
using namespace DirectX;

namespace
{
	class DrawCallList : public IDrawCallList
	{
		struct MyStrokeDrawCall : public StorkeRenderCall<RenderCall>
		{
			DirectX::XMFLOAT4X4 Transform;
		};
	public:
		DrawCallList(ID3D11DeviceContext* deviceContext, SwapChainUpdateContext& updateContext, D3D11ResourceManager* resMgr, RenderCommandBuffer* rcb)
			:_deviceContext(deviceContext), _resMgr(resMgr), _rcb(rcb), _updateContext(updateContext)
		{

		}

		// 通过 IDrawCallList 继承
		virtual void Draw(const DirectX::XMFLOAT4X4& modelTransform) override
		{
			using namespace D3D11;

			{
				auto model = _updateContext.Model.Map(_deviceContext.Get());
				model->Model = modelTransform;
			}
			float constants[4] = { 0 };
			UINT offset = 0;
			for (auto&& rc : _strokeRenderCalls)
			{
				auto vb = _resMgr->GetVertexBuffer(rc);
				_deviceContext->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &rc.Stride, &offset);

				{
					auto geo = _updateContext.Geometry.Map(_deviceContext.Get());
					ThrowIfNot(memcpy_s(geo->Color, sizeof(geo->Color), rc.Color, sizeof(rc.Color)) == 0, L"Cannot copy color.");
					geo->Thickness = rc.Thickness;
					geo->Transform = rc.Transform;
				}
				_deviceContext->Draw(rc.VertexCount, rc.StartVertex);
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
		ComPtr<ID3D11DeviceContext> _deviceContext;
		SwapChainUpdateContext& _updateContext;
		ComPtr<D3D11ResourceManager> _resMgr;
		ComPtr<RenderCommandBuffer> _rcb;
		std::vector<MyStrokeDrawCall> _strokeRenderCalls;
	};
}

D3D11ResourceManager::D3D11ResourceManager(WRL::ComPtr<D3D11DeviceContext>& deviceContext, SwapChainUpdateContext& updateContext)
	:ResourceManager(deviceContext), _updateContext(updateContext)
{
}

std::shared_ptr<IDrawCallList> D3D11ResourceManager::CreateDrawCallList(RenderCommandBuffer* rcb)
{
	auto ret = std::make_shared<DrawCallList>(_deviceContext->DeviceContext, _updateContext, this, rcb);
	ret->Initialize();
	return ret;
}