//
// MPF Platform
// Native Window
// 作者：SunnyCase
// 创建时间：2016-07-14
//
#pragma once
#include "../../inc/common.h"
#include "../../inc/NonCopyable.h"
#include "../../inc/WeakReferenceBase.h"

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

enum NativeWindowMessages
{
	NWM_Closing,
	NWM_Closed
};

typedef HRESULT (__stdcall *NativeWindowMessageHandler)(enum NativeWindowMessages);

class NativeWindow : private NonCopyable,
	public WeakReferenceBase<NativeWindow, WRL::RuntimeClassFlags<WRL::ClassicCom>, INativeWindow>
{
public:
	NativeWindow(NativeWindowMessageHandler messageHandler);
	virtual ~NativeWindow();

	// 通过 WeakReferenceBase 继承
	STDMETHODIMP get_HasMaximize(BOOL * value) override;
	STDMETHODIMP put_HasMaximize(BOOL value) override;
	STDMETHODIMP Show(void) override;
	STDMETHODIMP Hide(void) override;
	STDMETHODIMP get_Title(BSTR *value) override;
	STDMETHODIMP put_Title(BSTR value) override;
	STDMETHODIMP get_NativeHandle(INT_PTR * value) override;
	STDMETHODIMP Close() override;
	STDMETHODIMP Destroy() override;
private:
	void CreateWindow();
	LRESULT WindowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK NativeWindowProcWrapper(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);
	static void RegisterNativeWindowClass();
private:
	HWND _hWnd = nullptr;
	WeakRef<NativeWindow>* _weakRef;
	NativeWindowMessageHandler _messageHandler;
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateNativeWindow(NS_PLATFORM::NativeWindowMessageHandler messageHandler, NS_PLATFORM::INativeWindow** obj) noexcept;
}