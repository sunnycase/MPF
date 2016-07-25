//
// MPF Platform
// Native Window
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#include "stdafx.h"
#include "NativeWindow.h"
#include <atomic>
#include <algorithm>
using namespace WRL;

HRESULT __stdcall CreateNativeWindow(NS_PLATFORM::NativeWindowMessageHandler messageHandler, NS_PLATFORM::INativeWindow** obj) noexcept
{
	try
	{
		*obj = Make<NS_PLATFORM::NativeWindow>(messageHandler).Detach();
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

NativeWindow::NativeWindow(NativeWindowMessageHandler messageHandler)
	:_weakRef(new WeakRef<NativeWindow>(AsWeak())), _messageHandler(messageHandler)
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
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
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
		_messageHandler(NWM_Closing);
		std::for_each(_messageHandlers.begin(), _messageHandlers.end(), [](auto&& handler) {handler(NWM_Closing); });
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