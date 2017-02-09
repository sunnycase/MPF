using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Interop
{
    [ComImport]
    [Guid("790E0449-4EFC-4A99-8083-446D1DB91FAB")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    interface ISwapChainDrawingContext
    {
        void PushCamera([In]IResource camera);
        void PopCamera();
        void PushViewport([In]ref Rect viewport);
        void PopViewport();
    }
}
