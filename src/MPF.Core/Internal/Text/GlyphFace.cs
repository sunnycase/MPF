using MPF.Internal.FontCache;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Internal.Text
{
    internal class GlyphFace
    {
        internal FontFace FontFace { get; }
        public Geometry Geometry { get; }
        public FontMetrics FontMetrics => FontFace.FontMetrics;
        public GlyphMetrics GlyphMetrics { get; }

        public GlyphFace(Geometry geometry, FontFace fontFace, GlyphMetrics glyphMetrics)
        {
            Geometry = geometry;
            FontFace = fontFace;
            GlyphMetrics = glyphMetrics;
        }
    }
}
