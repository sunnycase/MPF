using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class MediaResourceManager
    {
        private static readonly Lazy<MediaResourceManager> _current = new Lazy<MediaResourceManager>(() => new MediaResourceManager(), true);

        public static MediaResourceManager Current => _current.Value;

        private readonly IResourceManager _resourceManager;
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

        public void UpdateSolidColorBrush(IResource res, ref ColorF color)
        {
            _resourceManager.UpdateSolidColorBrush(res, ref color);
        }

        public void UpdatePen(IResource res, float thickness, Brush brush)
        {
            _resourceManager.UpdatePen(res, thickness, ((IResourceProvider)brush)?.Resource);
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
