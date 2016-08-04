//
// MPF Platform
// OpenGL SwapChain
// 作者：SunnyCase
// 创建时间：2016-07-31
//
#include "stdafx.h"
#include "glewUtil.h"
#include <GL/wglew.h>
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

	GLint glGetIntegerv(GLenum name)
	{
		GLint value = 0;
		::glGetIntegerv(name,&value);
		return value;
	}

	void SelectAdapter()
	{
		if (WGLEW_NV_gpu_affinity)
		{
			for (int gpu = 0;; ++gpu)
			{
				HGPUNV hGPU;
				if (!wglEnumGpusNV(gpu, &hGPU))break;
				GPU_DEVICE gpuDevice{};
				gpuDevice.cb = sizeof(gpuDevice);
				if (!wglEnumGpuDevicesNV(hGPU, 0, &gpuDevice))continue;
			}
		}
	}

	void DumpAdapter(HDC hdc) noexcept
	{
		std::stringstream ss;
		ss << "MPF OpenGL Dump Info:\n"
			<< "Vendor: " << glGetString(GL_VENDOR) << '\n'
			<< "Renderer: " << glGetString(GL_RENDERER) << '\n'
			<< "Version: " << glGetString(GL_VERSION) << '\n'
			<< "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

		if (GLEW_NVX_gpu_memory_info)
		{
			ss << "Dedicated Video Memory: " << glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX) << " KB\n"
				<< "Total Avaliable Video Memory: " << glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX) << " KB\n"
				<< "Current Avaliable Video Memory: " << glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX) << " KB" << std::endl;
		}

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

	static bool dumped = false;
	if (!dumped)
	{
		DumpAdapter(_hdc);
		dumped = true;
	}
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
