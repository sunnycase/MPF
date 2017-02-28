//
// MPF Platform
// Image Manager
// 作者：SunnyCase
// 创建时间：2017-02-20
//
#include "stdafx.h"
#include "ImageManager.h"
#include <unordered_map>
using namespace WRL;
using namespace NS_PLATFORM;

ImageManager::ImageManager()
{
	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, &_wicFactory));
}

ImageManager::~ImageManager()
{
}

ComPtr<BitmapDecoder> ImageManager::CreateBitmapImage(IStream * stream)
{
	ComPtr<IWICBitmapDecoder> wicDecoder;
	ThrowIfFailed(_wicFactory->CreateDecoderFromStream(stream, nullptr, WICDecodeOptions::WICDecodeMetadataCacheOnDemand, &wicDecoder));
	return Make<BitmapDecoder>(wicDecoder.Get());
}

BitmapDecoder::BitmapDecoder(IWICBitmapDecoder* decoder)
	:_decoder(decoder)
{

}

STDMETHODIMP BitmapDecoder::get_FrameCount(UINT * value)
{
	return _decoder->GetFrameCount(value);
}

STDMETHODIMP BitmapDecoder::GetFrame(UINT id, IBitmapFrameDecode ** frame)
{
	try
	{
		ComPtr<IWICBitmapFrameDecode> wicFrame;
		ThrowIfFailed(_decoder->GetFrame(id, &wicFrame));
		*frame = Make<BitmapFrameDecode>(wicFrame.Get()).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

BitmapFrameDecode::BitmapFrameDecode(IWICBitmapFrameDecode* frame)
	:_frame(frame)
{

}

namespace std {
	template<> struct hash<GUID>
	{
		size_t operator()(const GUID& guid) const noexcept {
			const std::uint64_t* p = reinterpret_cast<const std::uint64_t*>(&guid);
			std::hash<std::uint64_t> hash;
			return hash(p[0]) ^ hash(p[1]);
		}
	};
}

namespace
{
	std::unordered_map<PixelFormat, WICPixelFormatGUID> formatToWic =
	{
		{ PixelFormat::PixelFormat1bppIndexed,		GUID_WICPixelFormat1bppIndexed		},
		{ PixelFormat::PixelFormat2bppIndexed,		GUID_WICPixelFormat2bppIndexed		},
		{ PixelFormat::PixelFormat4bppIndexed,		GUID_WICPixelFormat4bppIndexed		},
		{ PixelFormat::PixelFormat8bppIndexed,		GUID_WICPixelFormat8bppIndexed		},
		{ PixelFormat::PixelFormatBlackWhite,		GUID_WICPixelFormatBlackWhite		},
		{ PixelFormat::PixelFormat2bppGray,			GUID_WICPixelFormat2bppGray			},
		{ PixelFormat::PixelFormat4bppGray,			GUID_WICPixelFormat4bppGray			},
		{ PixelFormat::PixelFormat8bppGray,			GUID_WICPixelFormat8bppGray			},
		{ PixelFormat::PixelFormat8bppAlpha,		GUID_WICPixelFormat8bppAlpha		},
		{ PixelFormat::PixelFormat16bppBGR555,		GUID_WICPixelFormat16bppBGR555		},
		{ PixelFormat::PixelFormat16bppBGR565,		GUID_WICPixelFormat16bppBGR565		},
		{ PixelFormat::PixelFormat16bppGray,		GUID_WICPixelFormat16bppGray		},
		{ PixelFormat::PixelFormat24bppBGR,			GUID_WICPixelFormat24bppBGR			},
		{ PixelFormat::PixelFormat24bppRGB,			GUID_WICPixelFormat24bppRGB			},
		{ PixelFormat::PixelFormat32bppBGR,			GUID_WICPixelFormat32bppBGR			},
		{ PixelFormat::PixelFormat32bppBGRA,		GUID_WICPixelFormat32bppBGRA		},
		{ PixelFormat::PixelFormat32bppPBGRA,		GUID_WICPixelFormat32bppPBGRA		},
		{ PixelFormat::PixelFormat32bppGrayFloat,	GUID_WICPixelFormat32bppGrayFloat	},
		{ PixelFormat::PixelFormat32bppRGB,			GUID_WICPixelFormat32bppRGB			},
		{ PixelFormat::PixelFormat32bppRGBA,		GUID_WICPixelFormat32bppRGBA		},
		{ PixelFormat::PixelFormat32bppPRGBA,		GUID_WICPixelFormat32bppPRGBA		},
		{ PixelFormat::PixelFormat48bppRGB,			GUID_WICPixelFormat48bppRGB			},
		{ PixelFormat::PixelFormat48bppBGR,			GUID_WICPixelFormat48bppBGR			},
		{ PixelFormat::PixelFormat64bppRGB,			GUID_WICPixelFormat64bppRGB			},
	};

	std::unordered_map<WICPixelFormatGUID, PixelFormat> wicToFormat =
	{
		{ GUID_WICPixelFormat1bppIndexed,		PixelFormat::PixelFormat1bppIndexed		},
		{ GUID_WICPixelFormat2bppIndexed,		PixelFormat::PixelFormat2bppIndexed		},
		{ GUID_WICPixelFormat4bppIndexed,		PixelFormat::PixelFormat4bppIndexed		},
		{ GUID_WICPixelFormat8bppIndexed,		PixelFormat::PixelFormat8bppIndexed		},
		{ GUID_WICPixelFormatBlackWhite,		PixelFormat::PixelFormatBlackWhite		},
		{ GUID_WICPixelFormat2bppGray,			PixelFormat::PixelFormat2bppGray		},
		{ GUID_WICPixelFormat4bppGray,			PixelFormat::PixelFormat4bppGray		},
		{ GUID_WICPixelFormat8bppGray,			PixelFormat::PixelFormat8bppGray		},
		{ GUID_WICPixelFormat8bppAlpha,			PixelFormat::PixelFormat8bppAlpha		},
		{ GUID_WICPixelFormat16bppBGR555,		PixelFormat::PixelFormat16bppBGR555		},
		{ GUID_WICPixelFormat16bppBGR565,		PixelFormat::PixelFormat16bppBGR565		},
		{ GUID_WICPixelFormat16bppGray,			PixelFormat::PixelFormat16bppGray		},
		{ GUID_WICPixelFormat24bppBGR,			PixelFormat::PixelFormat24bppBGR		},
		{ GUID_WICPixelFormat24bppRGB,			PixelFormat::PixelFormat24bppRGB		},
		{ GUID_WICPixelFormat32bppBGR,			PixelFormat::PixelFormat32bppBGR		},
		{ GUID_WICPixelFormat32bppBGRA,			PixelFormat::PixelFormat32bppBGRA		},
		{ GUID_WICPixelFormat32bppPBGRA,		PixelFormat::PixelFormat32bppPBGRA		},
		{ GUID_WICPixelFormat32bppGrayFloat,	PixelFormat::PixelFormat32bppGrayFloat	},
		{ GUID_WICPixelFormat32bppRGB,			PixelFormat::PixelFormat32bppRGB		},
		{ GUID_WICPixelFormat32bppRGBA,			PixelFormat::PixelFormat32bppRGBA		},
		{ GUID_WICPixelFormat32bppPRGBA,		PixelFormat::PixelFormat32bppPRGBA		},
		{ GUID_WICPixelFormat48bppRGB,			PixelFormat::PixelFormat48bppRGB		},
		{ GUID_WICPixelFormat48bppBGR,			PixelFormat::PixelFormat48bppBGR		},
		{ GUID_WICPixelFormat64bppRGB,			PixelFormat::PixelFormat64bppRGB		},
	};
}

UINT ImageManager::GetBitsPerPixel(PixelFormat format)
{
	auto it = formatToWic.find(format);
	ThrowIfNot(it != formatToWic.end(), L"Invalid pixel format.");

	ComPtr<IWICComponentInfo> compInfo;
	ThrowIfFailed(_wicFactory->CreateComponentInfo(it->second, &compInfo));
	ComPtr<IWICPixelFormatInfo> formatInfo;
	ThrowIfFailed(compInfo.As(&formatInfo));

	UINT value;
	ThrowIfFailed(formatInfo->GetBitsPerPixel(&value));
	return value;
}

STDMETHODIMP BitmapFrameDecode::get_PixelFormat(PixelFormat * format)
{
	try
	{
		WICPixelFormatGUID wicFormat;
		ThrowIfFailed(_frame->GetPixelFormat(&wicFormat));
		
		auto it = wicToFormat.find(wicFormat);
		ThrowIfNot(it != wicToFormat.end(), L"Invalid pixel format.");
		*format = it->second;
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP BitmapFrameDecode::get_PixelSize(UInt32Size* size)
{
	try
	{
		return _frame->GetSize(&size->Width, &size->Height);
	}
	CATCH_ALL();
}

STDMETHODIMP BitmapFrameDecode::CopyPixels(const Int32Rect *srcRect, UINT stride, BYTE buffer[], UINT bufferSize)
{
	try
	{
		return _frame->CopyPixels(reinterpret_cast<const WICRect*>(srcRect), stride, bufferSize, buffer);
	}
	CATCH_ALL();
}