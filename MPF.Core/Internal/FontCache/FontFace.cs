using CodeProject.ObjectPool;
using MPF.Internal.Text;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Internal.FontCache
{
    internal class FontFace : IDisposable
    {
        private readonly IFontFamily _family;
        private readonly IFontFace _face;
        private readonly ParameterizedObjectPool<uint, PooledObjectWrapper<GlyphFace>> _glyphCache;

        public FontMetrics FontMetrics { get; }

        public FontFace(IFontFamily family, IFontFace face)
        {
            _family = family;
            _face = face;

            var fontMetrics = new FontMetrics();
            _face.get_FontMetrics(fontMetrics);
            FontMetrics = fontMetrics;

            _glyphCache = new ParameterizedObjectPool<uint, PooledObjectWrapper<GlyphFace>>(37, 4096, code =>
            {
                GlyphMetrics metrics;
                var resource = _face.CreateGlyphGeometry(MediaResourceManager.Current.Handle, code, out metrics);
                return new PooledObjectWrapper<GlyphFace>(new GlyphFace(Geometry.FromResource(resource), FontMetrics, metrics));
            });
            GC.AddMemoryPressure(10240);
        }

        public GlyphFace GetGlyph(uint code)
        {
            return _glyphCache.GetObject(code).InternalResource;
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                Marshal.ReleaseComObject(_face);
                GC.RemoveMemoryPressure(10240);
                disposedValue = true;
            }
        }

        ~FontFace()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
