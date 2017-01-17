//
// MPF Platform
// Input Manager
// 作者：SunnyCase
// 创建时间：2017-01-11
//
#pragma once
#include "../inc/common.h"
#include "../inc/NonCopyable.h"
#include <unordered_map>

DEFINE_NS_PLATFORM
#include "MPF.Platform_i.h"

class InputManager : private NonCopyable, public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IInputManager>
{
public:
	InputManager();

	static WRL::ComPtr<InputManager>& GetCurrent();

	STDMETHODIMP SetCallback(IInputManagerCallback* callback);
	STDMETHODIMP SetHIDAware(const HIDUsage usages[], UINT length);

	void DispatchHIDInputMessage(INativeWindow* window, DWORD time, DWORD mousePosition, HRAWINPUT hRawInput);
private:
	WRL::ComPtr<IInputManagerCallback> _callback;
	std::unordered_map<HANDLE, WRL::ComPtr<IInputDevice>> _inputDevices;
	WRL::Wrappers::CriticalSection _inputDevicesCS;
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall GetCurrentInputManager(NS_PLATFORM::IInputManager** obj) noexcept;
}