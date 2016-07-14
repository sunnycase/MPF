using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [ComImport]
    [Guid("2CABEDF6-D0CB-409D-A400-F03ACBF7663C")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface INativeApplication
    {
        void Run();
    }
}
