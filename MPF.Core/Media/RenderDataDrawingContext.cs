using MPF.Media3D;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class RenderDataDrawingContext : IDrawingContext, IDrawingContext3D
    {
        private readonly RenderData _renderData = new RenderData();
        private Matrix4x4 _identityMat4x4 = Matrix4x4.Identity;

        public RenderDataDrawingContext()
        {

        }

        public void DrawGeometry(Geometry geometry, Pen pen, Brush brush, ref Matrix3x2 transform)
        {
            var mat = new Matrix4x4(transform);
            _renderData.DrawGeometry(geometry, pen, brush, ref mat);
        }

        public void DrawGeometry(Geometry geometry, Pen pen, Brush brush)
        {
            _renderData.DrawGeometry(geometry, pen, brush, ref _identityMat4x4);
        }

        public void DrawGeometry3D(Geometry3D geometry, Pen pen, Brush brush, ref Matrix4x4 transform)
        {
            _renderData.DrawGeometry3D(geometry, pen, brush, ref transform);
        }

        public void DrawGeometry3D(Geometry3D geometry, Pen pen, Brush brush)
        {
            _renderData.DrawGeometry3D(geometry, pen, brush, ref _identityMat4x4);
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    CloseOverride(_renderData);
                    _renderData.Dispose();
                }
                disposedValue = true;
            }
        }

        protected virtual void CloseOverride(RenderData renderData)
        {

        }

        public void Dispose()
        {
            Dispose(true);
        }
        #endregion
    }
}
