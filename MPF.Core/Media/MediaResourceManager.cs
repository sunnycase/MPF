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
            _resourceManager = Platform.CreateResourceManager();
        }

        public IResource CreateResouce(ResourceType resType)
        {
            return _resourceManager.CreateResource(resType);
        }
    }
}
