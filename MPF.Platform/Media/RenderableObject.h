//
// MPF Platform
// Renderable Object
// 作者：SunnyCase
// 创建时间：2016-07-22
//
#pragma once
#include "../../inc/common.h"
#include "Geometry.h"
#include "ResourceRef.h"
#include "RenderCommandBuffer.h"
#include "DirectXMath.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

struct IDrawCallList;
typedef void(_stdcall *RenderableObjectAction)();

class RenderableObject : public ResourceBase
{
public:
	RenderableObject();

	void SetParent(RenderableObject* parent);
	void SetOffset(float x, float y);
	void SetContent(IRenderCommandBuffer* buffer);
	void Update();
	void Draw();
private:
	void SetBufferDirty();
	void UpdateTransform();
private:
	bool _isDirty;
	bool _isBufferDirty;
	RenderableObject* _parent;
	WRL::ComPtr<RenderCommandBuffer> _buffer;
	std::shared_ptr<IDrawCallList> _drawCallList;
	DirectX::XMMATRIX _modelTransform;
	DirectX::XMFLOAT3 _translation;
};

END_NS_PLATFORM