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

        public void UpdateResource(IResource res, ref LineGeometryData data)
        {
            _resourceManager.UpdateLineGeometry(res, ref data);
        }
    }
}
