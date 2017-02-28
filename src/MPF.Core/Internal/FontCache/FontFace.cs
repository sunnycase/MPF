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
        private readonly WeakConcurrentDictionary<uint, GlyphFace> _glyphCache = new WeakConcurrentDictionary<uint, GlyphFace>();

        public FontMetrics FontMetrics { get; }
        public uint UnitsPerEM => _face.UnitsPerEM;

        public FontFace(IFontFamily family, IFontFace face)
        {
            _family = family;
            _face = face;

            var fontMetrics = new FontMetrics();
            _face.get_FontMetrics(fontMetrics);
            FontMetrics = fontMetrics;
            
            GC.AddMemoryPressure(10240);
        }

        public GlyphFace GetGlyph(uint code)
        {
            return _glyphCache.GetOrAdd(code, k =>
            {
                GlyphMetrics metrics;
                var res = _face.CreateGlyphGeometry(MediaResourceManager.Current.Handle, k, out metrics);
                return new GlyphFace(Geometry.FromResource(res), this, metrics);
            });
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
