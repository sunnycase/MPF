using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class RenderDataDrawingContext : IDrawingContext
    {
        private readonly RenderData _renderData = new RenderData();

        public RenderDataDrawingContext()
        {

        }

        public void DrawGeometry(Geometry geometry, Pen pen)
        {
            _renderData.DrawGeometry(geometry, pen);
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
