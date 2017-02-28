using MPF.Interop;
using MPF.Media3D;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class RenderData : IDisposable
    {
        private readonly IRenderCommandBuffer _buffer;

        public RenderData()
        {
            _buffer = MediaResourceManager.Current.CreateRenderCommandBuffer();
            GC.AddMemoryPressure(512);
        }

        public void DrawGeometry(Geometry geometry, Pen pen, Brush brush, ref Matrix4x4 transform)
        {
            _buffer.DrawGeometry(((IResourceProvider)geometry).Resource, ((IResourceProvider)pen)?.Resource, ((IResourceProvider)brush)?.Resource, ref transform);
        }

        public void DrawGeometry3D(Geometry3D geometry, Material material, ref Matrix4x4 transform)
        {
            _buffer.DrawGeometry3D(((IResourceProvider)geometry).Resource, ((IResourceProvider)material)?.Resource, ref transform);
        }

        public IRenderCommandBuffer Close()
        {
            return _buffer;
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                GC.RemoveMemoryPressure(512);
                disposedValue = true;
            }
        }
        
         ~RenderData()
        {
            Dispose(false);
        }

        // 添加此代码以正确实现可处置模式。
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
