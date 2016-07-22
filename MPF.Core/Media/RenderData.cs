using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class RenderData
    {
        private readonly IRenderCommandBuffer _buffer;

        public RenderData()
        {
            _buffer = MediaResourceManager.Current.CreateRenderCommandBuffer();
        }

        public void DrawGeometry(Geometry geometry)
        {
            _buffer.DrawGeometry(((IResourceProvider)geometry).Resource);
        }

        public IRenderCommandBuffer Close()
        {
            return _buffer;
        }
    }
}
