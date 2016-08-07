using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    enum RenderBackendType : uint
    {
        RBT_Any,
        RBT_Direct3D9,
        RBT_OpenGL
    }

    [ComImport]
    [Guid("B82A3B78-E27D-41B3-8EDE-6190D2FFD261")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IDeviceContextCallback
    {
        void OnRender();
    }

    [ComImport]
    [Guid("CDFF5B2C-2CFB-4D80-A378-1B0F1AD95B41")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IDeviceContext
    {
        ISwapChain CreateSwapChain([MarshalAs(UnmanagedType.Interface), In]INativeWindow window);
        IRenderableObject CreateRenderableObject();
        IResourceManager CreateResourceManager();
    }
}
