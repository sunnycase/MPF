//
// MPF Platform
// Native Application
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#include "stdafx.h"
#include "NativeApplication.h"
#include "Input/InputManager.h"
using namespace WRL;

HRESULT __stdcall CreateNativeApplication(NS_PLATFORM::INativeApplication** obj) noexcept
{
	try
	{
		*obj = Make<NS_PLATFORM::NativeApplication>().Detach();
		return S_OK;
	}
	CATCH_ALL();
}

using namespace NS_PLATFORM;

WRL::Wrappers::CriticalSection NativeApplication::_eventsToWaitCS;
std::vector<HANDLE> NativeApplication::_eventsToWait;
std::vector<std::function<void()>> NativeApplication::_atExits;


NativeApplication::NativeApplication()
	:_updateCallback(nullptr)
{
	
}

NativeApplication::~NativeApplication()
{
}

HRESULT NativeApplication::Run(void)
{
	try
	{
		MSG msg;
		bool active = true;
		while (active)
		{
			try
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
						active = false;
					else
					{
						TranslateMessage(&msg);
						if (msg.message == WM_INPUT && !msg.hwnd)
							DispatchHIDInputMessage(msg);
						else
							DispatchMessage(&msg);
					}
				}
				else
				{
					auto updateCallback = _updateCallback;
					if (updateCallback)
						updateCallback();
					Sleep(100);
				}
			}
			catch (...) {}
		}
		{
			std::vector<std::function<void()>> atExits;
			std::vector<HANDLE> eventsToWait;
			{
				_eventsToWaitCS.Lock();
				std::swap(_atExits, atExits);
				std::swap(_eventsToWait, eventsToWait);
			}
			for (auto&& callback : atExits)
				callback();
			for (auto && handle : eventsToWait)
			{
				auto result = WaitForSingleObject(handle, 5000);
				ThrowWin32IfNot(result != WAIT_OBJECT_0 && result != WAIT_ABANDONED);
			}
		}
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeApplication::SetUpdateCallback(INT_PTR callback)
{
	_updateCallback = reinterpret_cast<void(__stdcall*)()>(callback);
	return S_OK;
}

void NativeApplication::AddEventToWait(HANDLE handle)
{
	_eventsToWaitCS.Lock();
	_eventsToWait.emplace_back(handle);
}

void NativeApplication::RemoveEventToWait(HANDLE handle)
{
	_eventsToWaitCS.Lock();
	_eventsToWait.erase(std::find(_eventsToWait.begin(), _eventsToWait.end(), handle));
}

void NativeApplication::AddAtExit(std::function<void()>&& callback)
{
	_eventsToWaitCS.Lock();
	_atExits.emplace_back(std::move(callback));
}

void NativeApplication::DispatchHIDInputMessage(const MSG& msg)
{
	InputManager::GetCurrent()->DispatchHIDInputMessage(nullptr, msg.time, GetMessagePos(), (HRAWINPUT)msg.lParam);
}