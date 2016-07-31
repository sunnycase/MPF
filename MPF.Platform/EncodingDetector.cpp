//
// Tomato Media Core
// 编码探测
// 作者：SunnyCase
// 创建时间：2015-12-31
//
#include "stdafx.h"
#include "../inc/EncodingDetector.h"

using namespace NS_PLATFORM;

EncodingDetector::EncodingDetector()
{
	ThrowIfFailed(CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&_multiLang)));
}

EncodingDetector::~EncodingDetector()
{
}

DWORD EncodingDetector::DetectCodePage(const std::string& text)
{
	INT len = text.length(), score = 2;
	auto infos = std::make_unique<DetectEncodingInfo[]>(score);
	ThrowIfFailed(_multiLang->DetectInputCodepage(MLDETECTCP_NONE, 0, const_cast<CHAR*>(text.data()), 
		&len, infos.get(), &score));
	if (score == 0) return CP_ACP;
	return infos[0].nCodePage;
}
