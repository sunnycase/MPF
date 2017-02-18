using MPF.Media3D;
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
        RT_Brush,
        RT_Pen,
        RT_Camera,
        RT_ShaderParameters,
        RT_Material,
        RT_BoxGeometry3D,
        RT_MeshGeometry3D,
        RT_SolidColorTexture,
        RT_Sampler,
        RT_ShadersGroup
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

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct BoxGeometry3DData
    {
        public Point3D Position;
        public float Width;
        public float Height;
        public float Depth;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    unsafe struct MeshGeometry3DData
    {
        public Point3D* Positions;
        public uint PositionsCount;
        public Vector3* Normals;
        public uint NormalsCount;
        public Point* TextureCoordinates;
        public uint TextureCoordinatesCount;
        public uint* Indices;
        public uint IndicesCount;
    }

    enum TextureAddress
    {
        TA_Wrap,
        TA_Mirror,
        TA_Clamp
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct SamplerData
    {
        public TextureAddress AddressU;
        public TextureAddress AddressV;
        public TextureAddress AddressW;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct ShadersGroupData
    {
        public IntPtr VertexShader;
        public uint VertexShaderLength;
        public IntPtr PixelShader;
        public uint PixelShaderLength;
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
        void UpdateBrush([In]IResource resource, [In]IResource texture, [In]IResource sampler);
        void UpdatePen([In]IResource resource, [In] float thickness, [In]IResource brush);
        void UpdateCamera([In]IResource resource, [In]ref Matrix4x4 viewMatrix, [In]ref Matrix4x4 projectionMatrix);
        void UpdateShaderParameters([In]IResource resource, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)]byte[] data, uint dataSize, [In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]IResource[] brushes, uint brushesCount);
        void UpdateMaterial([In]IResource resource, [In]IResource shader, [In] IResource shaderParams);
        void UpdateBoxGeometry3D([In]IResource resource, [In] ref BoxGeometry3DData data);
        void UpdateMeshGeometry3D([In]IResource resource, [In] ref MeshGeometry3DData data);
        void UpdateSolidColorTexture([In]IResource resource, [In]ref ColorF color);
        void UpdateSampler([In]IResource resource, [In]ref SamplerData data);
        void UpdateShadersGroup([In]IResource resource, [In]ref ShadersGroupData data);
    }
}
