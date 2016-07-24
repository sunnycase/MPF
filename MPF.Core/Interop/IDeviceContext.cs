using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    enum DeviceContextMessage
    {
        DCM_Render
    }

    internal delegate void DeviceContextMessageHandler(DeviceContextMessage message);

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
