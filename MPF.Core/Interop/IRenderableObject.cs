using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [Guid("7316E516-0357-4F99-82D8-181F2BD289C6")]
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IRenderableObject
    {
        void SetContent([MarshalAs(UnmanagedType.Interface), In]IRenderCommandBuffer buffer);
    }
}
