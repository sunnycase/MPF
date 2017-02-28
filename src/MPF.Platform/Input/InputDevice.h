//
// MPF Platform
// Input Device
// 作者：SunnyCase
// 创建时间：2017-01-11
//
#pragma once
#include "../inc/common.h"
#include "../inc/NonCopyable.h"

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class InputDevice : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IInputDevice>
{
public:
	InputDevice(HANDLE hDevice);

	STDMETHODIMP get_HIDUsage(HIDUsage *usage);
private:
	HANDLE _hDevice;
	RID_DEVICE_INFO _deviceInfo;
};

END_NS_PLATFORM