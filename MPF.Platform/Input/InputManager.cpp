//
// MPF Platform
// Input Manager
// 作者：SunnyCase
// 创建时间：2017-01-11
//
#include "stdafx.h"
#include "InputManager.h"
#include "InputDevice.h"
#include <hidsdi.h>
#include <windowsx.h>

using namespace WRL;

HRESULT __stdcall GetCurrentInputManager(NS_PLATFORM::IInputManager** obj) noexcept
{
	try
	{
		return NS_PLATFORM::InputManager::GetCurrent().CopyTo(obj);
	}
	CATCH_ALL();
}
using namespace NS_PLATFORM;

InputManager::InputManager()
	:_inputDevicesCS(500)
{
}

WRL::ComPtr<InputManager>& InputManager::GetCurrent()
{
	static auto inputManager = Make<InputManager>();
	return inputManager;
}

HRESULT InputManager::SetHIDAware(const HIDUsage usages[], UINT length)
{
	try
	{
		if (length)
		{
			auto devices = std::make_unique<RAWINPUTDEVICE[]>(length);
			for (size_t i = 0; i < length; i++)
			{
				const auto& usage = usages[i];
				auto& device = devices[i];
				device.hwndTarget = nullptr;
				device.dwFlags = 0;
				device.usUsagePage = usage.Page;
				device.usUsage = usage.Id;
			}
			ThrowWin32IfNot(RegisterRawInputDevices(devices.get(), length, sizeof(RAWINPUTDEVICE)));
		}
		else
		{
			ThrowWin32IfNot(GetRegisteredRawInputDevices(nullptr, &length, sizeof(RAWINPUTDEVICE)) == -1);
			ThrowWin32IfNot(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
			if (length)
			{
				auto devices = std::make_unique<RAWINPUTDEVICE[]>(length);
				ThrowWin32IfNot(GetRegisteredRawInputDevices(devices.get(), &length, sizeof(RAWINPUTDEVICE)));
				for (size_t i = 0; i < length; i++)
					devices[i].dwFlags = RIDEV_REMOVE;
				ThrowWin32IfNot(RegisterRawInputDevices(devices.get(), length, sizeof(RAWINPUTDEVICE)));
			}
		}
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT InputManager::SetCallback(IInputManagerCallback* callback)
{
	try
	{
		_callback = callback;
		return S_OK;
	}
	CATCH_ALL();
}

#define DHIM_IMPL1(buttonId) \
case RI_MOUSE_BUTTON_##buttonId##_DOWN:	 \
input.ChangedButton = ##buttonId##;		 \
input.ChangedButtonState = Pressed;		 \
break;									 \
case RI_MOUSE_BUTTON_##buttonId##_UP:	 \
input.ChangedButton = ##buttonId##;		 \
input.ChangedButtonState = Released;	 \
break

void InputManager::DispatchHIDInputMessage(HWND hWnd, DWORD time, DWORD mousePosition, HRAWINPUT hRawInput)
{
	UINT size;
	ThrowWin32IfNot(GetRawInputData(hRawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == 0);
	auto data = std::make_unique<byte[]>(size);
	ThrowWin32IfNot(GetRawInputData(hRawInput, RID_INPUT, data.get(), &size, sizeof(RAWINPUTHEADER)) == size);

	auto rawInput = reinterpret_cast<PRAWINPUT>(data.get());
	auto callback = _callback;
	if (callback)
	{
		auto hDevice = rawInput->header.hDevice;
		ComPtr<IInputDevice> inputDevice;
		{
			auto locker = _inputDevicesCS.Lock();
			auto it = _inputDevices.find(hDevice);
			if (it == _inputDevices.end())
			{
				inputDevice = Make<InputDevice>(hDevice);
				_inputDevices.emplace(hDevice, inputDevice);
			}
			else
				inputDevice = it->second;
		}
		switch (rawInput->header.dwType)
		{
		case RIM_TYPEMOUSE:
		{
			HIDMouseInput input{};
			const auto& data = rawInput->data.mouse;
			input.CursorX = GET_X_LPARAM(mousePosition);
			input.CursorY = GET_Y_LPARAM(mousePosition);
			input.Time = time;
			switch (data.usButtonFlags)
			{
				DHIM_IMPL1(1);
				DHIM_IMPL1(2);
				DHIM_IMPL1(3);
				DHIM_IMPL1(4);
				DHIM_IMPL1(5);
			case RI_MOUSE_WHEEL:
				input.ChangedButton = 6;
				input.WheelDelta = (short)data.usButtonData;
				break;
			default:
				break;
			}
			input.XDelta = data.lLastX;
			input.YDelta = data.lLastY;
			callback->OnMouseInput(inputDevice.Get(), &input);
			break;
		}
		default:
			break;
		}
	}
}