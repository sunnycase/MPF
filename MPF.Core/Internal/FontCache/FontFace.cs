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

        public FontFace(IFontFamily family, IFontFace face)
        {
            _family = family;
            _face = face;
            GC.AddMemoryPressure(10240);
        }

        public Geometry GetGlyphGeometry(char code)
        {
            var resource = _face.CreateGlyphGeometry(MediaResourceManager.Current.Handle, (uint)code);
            return Geometry.FromResource(resource);
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
