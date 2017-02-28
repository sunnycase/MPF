//
// MPF Platform
// Font Family
// 作者：SunnyCase
// 创建时间：2016-08-09
//
#include "stdafx.h"
#include "FontFamily.h"
using namespace WRL;

HRESULT __stdcall GetSystemFontFaceLocation(BSTR faceName, BSTR* location) noexcept
{
	try
	{
		static const LPWSTR fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
		HKEY hKey;
		std::wstring wsFaceName(faceName, SysStringLen(faceName));

		// Open Windows font registry key
		ThrowWin32IfNot(RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS);
		auto fin = make_finalizer([&] {RegCloseKey(hKey); });

		DWORD maxValueNameSize, maxValueDataSize;
		ThrowWin32IfNot(RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0) == ERROR_SUCCESS);

		DWORD valueIndex = 0;
		auto valueName = std::make_unique<WCHAR[]>(maxValueNameSize);
		auto valueData = std::make_unique<BYTE[]>(maxValueDataSize);
		DWORD valueNameSize, valueDataSize, valueType;
		std::wstring wsFontFile;

		LONG result;
		// Look for a matching font name
		do {

			wsFontFile.clear();
			valueDataSize = maxValueDataSize;
			valueNameSize = maxValueNameSize;

			result = RegEnumValue(hKey, valueIndex, valueName.get(), &valueNameSize, 0, &valueType, valueData.get(), &valueDataSize);

			valueIndex++;

			if (result != ERROR_SUCCESS || valueType != REG_SZ) {
				continue;
			}

			std::wstring wsValueName(valueName.get(), valueNameSize);

			// Found a match
			if (_wcsnicmp(wsFaceName.c_str(), wsValueName.c_str(), wsFaceName.length()) == 0) {

				wsFontFile.assign((LPWSTR)valueData.get(), valueDataSize);
				break;
			}
		} while (result != ERROR_NO_MORE_ITEMS);

		ThrowIfNot(!wsFontFile.empty(), L"Cannot find font file.");
		
		if(wsFontFile.length() >= 2 && wsFontFile[1] == L':')
			*location = _bstr_t(wsFontFile.c_str()).Detach();
		else
		{
			// Build full font file path
			WCHAR winDir[MAX_PATH];
			ThrowWin32IfNot(GetWindowsDirectory(winDir, MAX_PATH));

			std::wstringstream ss;
			ss << winDir << "\\Fonts\\" << wsFontFile;
			*location = _bstr_t(ss.str().c_str()).Detach();
		}
		return S_OK;
	}
	CATCH_ALL();
}