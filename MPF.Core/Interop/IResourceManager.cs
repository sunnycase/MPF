using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    enum ResourceType
    {
        RT_LineGeometry,
        RT_RectangleGeometry,
        RT_PathGeometry,
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

    namespace PathGeometrySegments
    {
        enum Operations
        {
            MoveTo,
            LineTo,
            ArcTo
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct MoveTo
        {
            public Operations Type;
            public Point Point;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct LineTo
        {
            public Operations Type;
            public Point Point;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        struct ArcTo
        {
            public Operations Type;
            public Point Point;
            public float Angle;
        }
    }

    [Guid("C8E784D3-3EBD-40D0-A421-55B3B52EF590")]
    [ComImport]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IResourceManager
    {
        IRenderCommandBuffer CreateRenderCommandBuffer();
        IFontFace CreateFontFaceFromMemory([In]IntPtr buffer, [In]ulong size, [In] uint faceIndex);
        IResource CreateResource([In]ResourceType resType);
        void UpdateLineGeometry([In]IResource resource, [In] ref LineGeometryData data);
        void UpdateRectangleGeometry([In]IResource resource, [In] ref RectangleGeometryData data);
        void UpdatePathGeometry([In]IResource resource, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] data, [In] uint length);
        void UpdateSolidColorBrush([In]IResource resource, [In] ref ColorF color);
        void UpdatePen([In]IResource resource, [In] float thickness, [In]IResource brush);
    }
}
