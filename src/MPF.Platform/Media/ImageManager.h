//
// MPF Platform
// Image Manager
// 作者：SunnyCase
// 创建时间：2017-02-20
//
#pragma once
#include "../../inc/common.h"
#include <wincodec.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class BitmapFrameDecode : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IBitmapFrameDecode>
{
public:
	BitmapFrameDecode(IWICBitmapFrameDecode* frame);

	STDMETHODIMP get_PixelFormat(PixelFormat* format) override;
	STDMETHODIMP get_PixelSize(UInt32Size* size) override;
	STDMETHODIMP CopyPixels(const Int32Rect *srcRect, UINT stride, BYTE buffer[], UINT bufferSize) override;
private:
	WRL::ComPtr<IWICBitmapFrameDecode> _frame;
};

class BitmapDecoder : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IBitmapDecoder>
{
public:
	BitmapDecoder(IWICBitmapDecoder* decoder);

	STDMETHODIMP get_FrameCount(UINT* value) override;
	STDMETHODIMP GetFrame(UINT id, IBitmapFrameDecode** frame) override;
private:
	WRL::ComPtr<IWICBitmapDecoder> _decoder;
};

class ImageManager : std::enable_shared_from_this<ImageManager>
{
public:
	ImageManager();
	~ImageManager();

	WRL::ComPtr<BitmapDecoder> CreateBitmapImage(IStream* stream);
	UINT GetBitsPerPixel(PixelFormat format);
private:
	WRL::ComPtr<IWICImagingFactory> _wicFactory;
};

END_NS_PLATFORM