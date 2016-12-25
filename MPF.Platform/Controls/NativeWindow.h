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
#include <functional>
#include <vector>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

enum NativeWindowMessages
{
	NWM_Closing,
	NWM_Closed,
	NWM_SizeChanged
};

typedef HRESULT (__stdcall *NativeWindowMessageHandler)(enum NativeWindowMessages);

struct DECLSPEC_UUID("53DD3D11-22DC-4C82-A703-63ABFD831058") INativeWindowIntern : public IUnknown
{
	virtual void AppendMessageHandler(std::function<void(NativeWindowMessages)>&& messageHandler) = 0;
};

class NativeWindow : private NonCopyable,
	public WeakReferenceBase<NativeWindow, WRL::RuntimeClassFlags<WRL::ClassicCom>, INativeWindow, INativeWindowIntern>
{
public:
	NativeWindow(INativeWindowCallback* callback);
	virtual ~NativeWindow();

	// 通过 WeakReferenceBase 继承
	STDMETHODIMP get_HasMaximize(BOOL * value) override;
	STDMETHODIMP put_HasMaximize(BOOL value) override;
	STDMETHODIMP Show(void) override;
	STDMETHODIMP Hide(void) override;
	STDMETHODIMP get_Title(BSTR *value) override;
	STDMETHODIMP put_Title(BSTR value) override;
	STDMETHODIMP get_NativeHandle(INT_PTR * value) override;
	STDMETHODIMP get_Location(Point *value);
	STDMETHODIMP put_Location(Point value);
	STDMETHODIMP get_Size(Point *value);
	STDMETHODIMP put_Size(Point value);
	STDMETHODIMP get_ClientSize(Point *value);
	STDMETHODIMP Close() override;
	STDMETHODIMP Destroy() override;

	virtual void AppendMessageHandler(std::function<void(NativeWindowMessages)>&& messageHandler) override;
private:
	void CreateWindow();
	LRESULT WindowProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK NativeWindowProcWrapper(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);
	static void RegisterNativeWindowClass();
	void BroadcastMessage(NativeWindowMessages message);
private:
	WRL::ComPtr<INativeWindowCallback> _callback;
	HWND _hWnd = nullptr;
	WeakRef<NativeWindow>* _weakRef;
	std::vector<std::function<void(NativeWindowMessages)>> _messageHandlers;
};

END_NS_PLATFORM

extern "C"
{
	HRESULT MPF_PLATFORM_API __stdcall CreateNativeWindow(NS_PLATFORM::INativeWindowCallback* callback, NS_PLATFORM::INativeWindow** obj) noexcept;
}