using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    enum ResourceType
    {
        RT_LineGeometry
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct LineGeometryData
    {
        public Point StartPoint;
        public Point EndPoint;
    }

    [Guid("C8E784D3-3EBD-40D0-A421-55B3B52EF590")]
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IResourceManager
    {
        IResource CreateResource([In]ResourceType resType);
        void UpdateLineGeometry([In]IResource resouce, [In] ref LineGeometryData data);
    }
}
