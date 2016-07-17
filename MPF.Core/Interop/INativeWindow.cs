using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    internal enum NativeWindowMessages
    {
        Closing,
        Closed
    }
    
    delegate void NativeWindowMessageHandler(NativeWindowMessages message);

    [ComImport]
    [Guid("2705FA1D-68BA-4E2B-92D4-846AE0A8D208")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface INativeWindow
    {
        bool HasMaximize { [return: MarshalAs(UnmanagedType.Bool)]get; [param: MarshalAs(UnmanagedType.Bool)]set; }
        string Title { [return: MarshalAs(UnmanagedType.BStr)]get; [param: MarshalAs(UnmanagedType.BStr)]set; }
        IntPtr NativeHandle { get; }
        void Show();
        void Hide();
        void Close();
        void Destroy();
    }
}
