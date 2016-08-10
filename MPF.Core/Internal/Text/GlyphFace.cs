using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Internal.Text
{
    internal class GlyphFace
    {
        public Geometry Geometry { get; }
        public FontMetrics FontMetrics { get; }
        public GlyphMetrics GlyphMetrics { get; }

        public GlyphFace(Geometry geometry, FontMetrics fontMetrics, GlyphMetrics glyphMetrics)
        {
            Geometry = geometry;
            FontMetrics = fontMetrics;
            GlyphMetrics = glyphMetrics;
        }
    }
}
