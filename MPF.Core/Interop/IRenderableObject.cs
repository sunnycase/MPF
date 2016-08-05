using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [Flags]
    internal enum RenderableObjectFlags : uint
    {
        None = 0,
        MeasureDirty = 1
    }

    [Guid("7316E516-0357-4F99-82D8-181F2BD289C6")]
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IRenderableObject
    {
        void SetContent([MarshalAs(UnmanagedType.Interface), In]IRenderCommandBuffer buffer);
        void SetMeasureCallback([MarshalAs(UnmanagedType.FunctionPtr), In]Action callback);
        void SetFlags(RenderableObjectFlags flags);
        void Render();
    }
}
