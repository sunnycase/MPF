//
// MPF Platform
// OpenGL SwapChain
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#include "stdafx.h"
#include "glewUtil.h"
#include "GLSwapChain.h"
using namespace WRL;
using namespace NS_PLATFORM;

namespace
{
	HWND GetNativeHandle(INativeWindow* nativeWindow)
	{
		INT_PTR handle;
		ThrowIfFailed(nativeWindow->get_NativeHandle(&handle));
		return reinterpret_cast<HWND>(handle);
	}

	const GLubyte* glGetString(GLenum name)
	{
		auto str = ::glGetString(name);
		return str ? str : reinterpret_cast<const GLubyte*>("Unknown");
	}

	void DumpAdapter() noexcept
	{
		std::stringstream ss;
		ss << "MPF OpenGL Dump Info:\n"
			<< "Vendor: " << glGetString(GL_VENDOR) << '\n'
			<< "Renderer: " << glGetString(GL_RENDER) << '\n'
			<< "Version: " << glGetString(GL_VERSION) << '\n'
			<< "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		puts(ss.str().c_str());
	}
}

GLSwapChainBase::GLSwapChainBase(INativeWindow * window)
	:_hWnd(GetNativeHandle(window))
{
	_hdc = GetDC(_hWnd);
	ThrowWin32IfNot(_hdc);

	static const PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		24,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer  
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};
	auto iPixelFormat = ChoosePixelFormat(_hdc, &pfd);
	ThrowWin32IfNot(SetPixelFormat(_hdc, iPixelFormat, &pfd));
	_glCtx = wglCreateContext(_hdc);
	ThrowWin32IfNot(_glCtx);
	ThrowWin32IfNot(wglMakeCurrent(_hdc, _glCtx));
	ThrowGlewIfNotOK(glewInit());
	DumpAdapter();
}

GLSwapChainBase::~GLSwapChainBase()
{
	wglDeleteContext(_glCtx);
	ReleaseDC(_hWnd, _hdc);
}

HRESULT GLSwapChainBase::SetCallback(ISwapChainCallback * callback)
{
	try
	{
		_callback = callback;
		return S_OK;
	}
	CATCH_ALL();
}
