using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [ComImport]
    [Guid("D1BCE22E-1DA3-4183-B870-814EE56E1D37")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface ISwapChain
    {
        void SetCallback([MarshalAs(UnmanagedType.Interface), In] ISwapChainCallback callback);
    }
}
