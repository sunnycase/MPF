using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    enum ResourceType
    {
        RT_LineGeometry,
        RT_RectangleGeometry,
        RT_SolidColorBrush,
        RT_Pen
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct LineGeometryData
    {
        public Point StartPoint;
        public Point EndPoint;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct RectangleGeometryData
    {
        public Point LeftTop;
        public Point RightBottom;
    }

    [Guid("C8E784D3-3EBD-40D0-A421-55B3B52EF590")]
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IResourceManager
    {
        IRenderCommandBuffer CreateRenderCommandBuffer();
        IResource CreateResource([In]ResourceType resType);
        void UpdateLineGeometry([In]IResource resouce, [In] ref LineGeometryData data);
        void UpdateRectangleGeometry([In]IResource resouce, [In] ref RectangleGeometryData data);
        void UpdateSolidColorBrush([In]IResource resource, [In] ref ColorF color);
        void UpdatePen([In]IResource resource, [In] float thickness, [In]IResource brush);
    }
}
