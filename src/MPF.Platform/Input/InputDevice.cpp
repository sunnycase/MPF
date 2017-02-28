//
// MPF Platform
// Input Device
// 作者：SunnyCase
// 创建时间：2017-01-11
//
#include "stdafx.h"
#include "InputDevice.h"
#include <hidusage.h>

using namespace WRL;
using namespace NS_PLATFORM;

InputDevice::InputDevice(HANDLE hDevice)
	:_hDevice(hDevice)
{
	if (!hDevice)
		ThrowIfFailed(E_INVALIDARG);
	
	_deviceInfo.cbSize = sizeof(_deviceInfo);
	UINT size = sizeof(_deviceInfo);
	ThrowWin32IfNot(GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &_deviceInfo, &size) > 0);
}

HRESULT InputDevice::get_HIDUsage(HIDUsage * usage)
{
	switch (_deviceInfo.dwType)
	{
	case RIM_TYPEMOUSE:
		usage->Page = HID_USAGE_PAGE_GENERIC;
		usage->Id = HID_USAGE_GENERIC_MOUSE;
		break;
	case RIM_TYPEKEYBOARD:
		usage->Page = HID_USAGE_PAGE_GENERIC;
		usage->Id = HID_USAGE_GENERIC_KEYBOARD;
		break;
	case RIM_TYPEHID:
		usage->Page = _deviceInfo.hid.usUsagePage;
		usage->Id = _deviceInfo.hid.usUsage;
		break;
	default:
		return E_UNEXPECTED;
	}
	return S_OK;
}
