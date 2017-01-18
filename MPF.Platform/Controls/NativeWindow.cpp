//
// MPF Platform
// Native Window
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#include "stdafx.h"
#include "NativeWindow.h"
#include "../Input/InputManager.h"
#include <atomic>
#include <algorithm>
#include <windowsx.h>
using namespace WRL;

HRESULT __stdcall CreateNativeWindow(NS_PLATFORM::INativeWindowCallback* callback, NS_PLATFORM::INativeWindow** obj) noexcept
{
	try
	{
		*obj = Make<NS_PLATFORM::NativeWindow>(callback).Detach();
		return S_OK;
	}
	CATCH_ALL();
}

using namespace NS_PLATFORM;

namespace
{
	const wchar_t MPFWindowClassName[] = L"MPF_Window_Wrapper";
	enum MPFWindowExtraIds : int
	{
		WndIdx_WeakRefPtr,
		MPFWindowExtraIds_Count
	};

	static_assert(sizeof(LONG_PTR) >= sizeof(void*), "sizeof(LONG_PTR) >= sizeof(void*)");
}

void NativeWindow::RegisterNativeWindowClass()
{
	static std::atomic<bool> init = false;
	if (!init.exchange(true))
	{
		WNDCLASSEX wndCls{};

		wndCls.cbSize = sizeof(wndCls);
		wndCls.hInstance = GetModuleHandle(nullptr);
		wndCls.style = CS_HREDRAW | CS_VREDRAW;
		wndCls.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndCls.lpszClassName = MPFWindowClassName;
		wndCls.lpfnWndProc = NativeWindowProcWrapper;
		wndCls.cbWndExtra = sizeof(LONG_PTR) * MPFWindowExtraIds_Count;

		ThrowWin32IfNot(RegisterClassEx(&wndCls));
	}
}

void NativeWindow::BroadcastMessage(NativeWindowMessages message)
{
	std::for_each(_messageHandlers.begin(), _messageHandlers.end(), [=](auto&& handler) {handler(message); });
}

HRESULT NativeWindow::get_Location(MPF::Platform::Point *value)
{
	try
	{
		RECT rect;
		ThrowWin32IfNot(GetWindowRect(_hWnd, &rect));
		value->X = float(rect.left);
		value->Y = float(rect.top);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::put_Location(MPF::Platform::Point value)
{
	try
	{
		ThrowWin32IfNot(SetWindowPos(_hWnd, nullptr, int(value.X), int(value.Y), 0, 0, SWP_NOSIZE));
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::get_Size(MPF::Platform::Point *value)
{
	try
	{
		RECT rect;
		ThrowWin32IfNot(GetWindowRect(_hWnd, &rect));
		value->X = float(rect.right - rect.left);
		value->Y = float(rect.bottom - rect.top);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::put_Size(MPF::Platform::Point value)
{
	try
	{
		ThrowWin32IfNot(SetWindowPos(_hWnd, nullptr, 0, 0, int(value.X), int(value.Y), SWP_NOMOVE));
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::get_ClientSize(MPF::Platform::Point *value)
{
	try
	{
		RECT rect;
		ThrowWin32IfNot(GetClientRect(_hWnd, &rect));
		value->X = float(rect.right - rect.left);
		value->Y = float(rect.bottom - rect.top);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::get_NativeHandle(INT_PTR * value)
{
	*value = reinterpret_cast<INT_PTR>(_hWnd);
	return S_OK;
}

STDMETHODIMP NativeWindow::Close()
{
	try
	{
		ThrowWin32IfNot(PostMessage(_hWnd, WM_CLOSE, 0, 0));
		return S_OK;
	}
	CATCH_ALL();
}

STDMETHODIMP NativeWindow::Destroy()
{
	try
	{
		ThrowWin32IfNot(PostMessage(_hWnd, WM_DESTROY, 0, 0));
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::get_Title(BSTR *value)
{
	try
	{
		if (auto len = GetWindowTextLength(_hWnd))
		{
			std::wstring str(len, L'\0');
			ThrowWin32IfNot(GetWindowText(_hWnd, const_cast<LPWSTR>(str.data()), str.length()));
			*value = bstr_t(str.c_str()).Detach();
		}
		else
			*value = bstr_t(L"").Detach();
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::put_Title(BSTR value)
{
	try
	{
		ThrowWin32IfNot(SetWindowText(_hWnd, value));
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::get_HasMaximize(BOOL * value)
{
	try
	{
		auto ret = GetWindowLong(_hWnd, GWL_STYLE);
		ThrowWin32IfNot(ret);
		*value = (ret & WS_MAXIMIZEBOX) != 0;
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::put_HasMaximize(BOOL value)
{
	try
	{
		auto old = GetWindowLong(_hWnd, GWL_STYLE);
		ThrowWin32IfNot(old);
		old = value ? (old | WS_MAXIMIZEBOX) : (old & ~WS_MAXIMIZEBOX);
		auto ret = SetWindowLong(_hWnd, GWL_STYLE, old);
		ThrowWin32IfNot(ret);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::Show(void)
{
	try
	{
		ThrowWin32IfNot(ShowWindow(_hWnd, SW_SHOW));
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT NativeWindow::Hide(void)
{
	try
	{
		ThrowWin32IfNot(ShowWindow(_hWnd, SW_HIDE));
		return S_OK;
	}
	CATCH_ALL();
}

LRESULT CALLBACK NativeWindow::NativeWindowProcWrapper(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		auto windowRef = reinterpret_cast<WeakRef<NativeWindow>*>(GetWindowLongPtr(hWnd, WndIdx_WeakRefPtr));

		if (uMsg == WM_NCCREATE)
		{
			auto createParam = (LPCREATESTRUCT)lParam;
			windowRef = reinterpret_cast<WeakRef<NativeWindow>*>(createParam->lpCreateParams);
			SetWindowLongPtr(hWnd, WndIdx_WeakRefPtr, reinterpret_cast<LONG_PTR>(windowRef));
		}

		if (windowRef)
		{
			if (auto window = windowRef->Resolve())
				return window->WindowProc(hWnd, uMsg, wParam, lParam);
			else
			{
				SetWindowLongPtr(hWnd, WndIdx_WeakRefPtr, NULL);
				delete windowRef;
			}
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	CATCH_ALL();
}

NativeWindow::NativeWindow(INativeWindowCallback* callback)
	:_weakRef(new WeakRef<NativeWindow>(AsWeak())), _callback(callback)
{
	RegisterNativeWindowClass();
	CreateWindow();
}

NativeWindow::~NativeWindow()
{
}

void NativeWindow::CreateWindow()
{
	if (!_hWnd)
	{
		_hWnd = CreateWindowEx(0, MPFWindowClassName, nullptr,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, GetModuleHandle(nullptr), _weakRef);
		ThrowWin32IfNot(_hWnd);
	}
}

LRESULT NativeWindow::WindowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		_callback->OnClosing();
		BroadcastMessage(NWM_Closing);
		return 0;
	case WM_WINDOWPOSCHANGED:
	{
		auto args = reinterpret_cast<WINDOWPOS*>(lParam);
		bool hasChange = !(args->flags & SWP_NOMOVE) || !(args->flags & SWP_NOSIZE);
		if (hasChange)
		{
			if (!(args->flags & SWP_NOMOVE))
				_callback->OnLocationChanged(Point{ float(args->x),float(args->y) });
			if (!(args->flags & SWP_NOSIZE))
				_callback->OnSizeChanged(Point{ float(args->cx),float(args->cy) });
			BroadcastMessage(NWM_SizeChanged);
			return 0;
		}
		break;
	}
	case WM_INPUT:
		DispatchHIDInputMessage(lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void NativeWindow::AppendMessageHandler(std::function<void(NativeWindowMessages)>&& messageHandler)
{
	_messageHandlers.emplace_back(std::move(messageHandler));
}

void NativeWindow::DispatchHIDInputMessage(LPARAM lParam)
{
	const auto pos = GetMessagePos();
	POINT cursorPt{ GET_X_LPARAM(pos), GET_Y_LPARAM(pos) };
	ScreenToClient(_hWnd, &cursorPt);

	InputManager::GetCurrent()->DispatchHIDInputMessage(this, GetMessageTime(), MAKELONG(cursorPt.x, cursorPt.y), (HRAWINPUT)lParam);
}