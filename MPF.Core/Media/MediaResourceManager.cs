using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class MediaResourceManager
    {
        private static readonly Lazy<MediaResourceManager> _current = new Lazy<MediaResourceManager>(() => new MediaResourceManager(), true);

        public static MediaResourceManager Current => _current.Value;

        private readonly IResourceManager _resourceManager;
        public IResourceManager Handle => _resourceManager;

        private MediaResourceManager()
        {
            _resourceManager = DeviceContext.Current.CreateResourceManager();
        }

        public IResource CreateResouce(ResourceType resType)
        {
            return _resourceManager.CreateResource(resType);
        }

        public IRenderCommandBuffer CreateRenderCommandBuffer()
        {
            return _resourceManager.CreateRenderCommandBuffer();
        }

        public void UpdateLineGeometry(IResource res, ref LineGeometryData data)
        {
            _resourceManager.UpdateLineGeometry(res, ref data);
        }

        public void UpdateRectangleGeometry(IResource res, ref RectangleGeometryData data)
        {
            _resourceManager.UpdateRectangleGeometry(res, ref data);
        }

        public void UpdatePathGeometry(IResource res, byte[] data)
        {
            _resourceManager.UpdatePathGeometry(res, data, (uint)(data?.Length ?? 0));
        }

        public void UpdateBrush(IResource res, IResource texture, IResource sampler)
        {
            _resourceManager.UpdateBrush(res, texture, sampler);
        }

        public void UpdatePen(IResource res, float thickness, Brush brush)
        {
            _resourceManager.UpdatePen(res, thickness, ((IResourceProvider)brush)?.Resource);
        }

        public void UpdateCamera(IResource res, ref Matrix4x4 viewMatrix, ref Matrix4x4 projectionMatrix)
        {
            _resourceManager.UpdateCamera(res, ref viewMatrix, ref projectionMatrix);
        }

        public void UpdateBoxGeometry3D(IResource res, ref BoxGeometry3DData data)
        {
            _resourceManager.UpdateBoxGeometry3D(res, ref data);
        }

        public void UpdateMeshGeometry3D(IResource res, ref MeshGeometry3DData data)
        {
            _resourceManager.UpdateMeshGeometry3D(res, ref data);
        }

        public void UpdateSolidColorTexture(IResource res, ref ColorF color)
        {
            _resourceManager.UpdateSolidColorTexture(res, color);
        }

        public void UpdateSampler(IResource res, ref SamplerData data)
        {
            _resourceManager.UpdateSampler(res, ref data);
        }

        public void UpdateShadersGroup(IResource res, ref ShadersGroupData data)
        {
            _resourceManager.UpdateShadersGroup(res, ref data);
        }

        public void UpdateShaderParameters(IResource resource, byte[] data, IResource[] brushes)
        {
            _resourceManager.UpdateShaderParameters(resource, data, (uint)data.Length, brushes, (uint)brushes.Length);
        }

        public void UpdateMaterial(IResource resource, IResource shader, IResource shaderParams)
        {
            _resourceManager.UpdateMaterial(resource, shader, shaderParams);
        }

        public IFontFace CreateFontFaceFromMemory(IntPtr buffer, ulong size, uint faceIndex)
        {
            return _resourceManager.CreateFontFaceFromMemory(buffer, size, faceIndex);
        }
    }

    internal static class MediaResourceManagerExtensions
    {
        public static Lazy<IResource> CreateResource(this DependencyObject obj, ResourceType type)
        {
            return new Lazy<IResource>(() => MediaResourceManager.Current.CreateResouce(type));
        }
    }
}
