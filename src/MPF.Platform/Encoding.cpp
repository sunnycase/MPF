//
// Tomato Media
// 字符编码
// 
// (c) SunnyCase 
// 创建日期 2015-03-18
#include "stdafx.h"
#include "../inc/Encoding.h"
#include "../inc/EncodingDetector.h"

using namespace NS_PLATFORM;

std::wstring NS_PLATFORM::s2ws(const std::string& str, UINT codePage)
{
	auto len = MultiByteToWideChar(codePage, 0, str.data(), str.size(), nullptr, 0);
	std::wstring ws(len, 0);
	MultiByteToWideChar(codePage, 0, str.data(), str.size(), &ws[0], len);

	return ws;
}

std::string NS_PLATFORM::ws2s(const std::wstring& str, UINT codePage)
{
	auto len = WideCharToMultiByte(codePage, 0, str.data(), str.size(), nullptr, 0, nullptr, nullptr);
	std::string s(len, 0);
	WideCharToMultiByte(codePage, 0, str.data(), str.size(), &s[0], len, nullptr, nullptr);

	return s;
}

std::wstring NS_PLATFORM::s2ws(const std::string& str)
{
	static EncodingDetector encDet;
	if (str.empty())
		return{};
	else
		return s2ws(str, encDet.DetectCodePage(str));
}