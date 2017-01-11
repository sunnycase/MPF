using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using MPF.Input;

namespace MPF.Interop
{
    [ComImport]
    [Guid("5B03D673-8837-4525-9E49-800E85CE314E")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IInputDevice
    {
        HIDUsage HIDUsage { get; }
    }
}
