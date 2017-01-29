using MPF.Media;
using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class GlyphRun
    {
        private readonly FontFamily _fontFamily;
        private readonly IReadOnlyList<char> _characters;
        private readonly IReadOnlyList<GlyphDrawingEntry> _drawingEntries;
        private readonly float _renderingEmSize;
        private float _width, _height;

        public Size Size => new Size(_width, _height);

        public GlyphRun(FontFamily fontFamily, IReadOnlyList<char> characters, float renderingEmSize)
        {
            _fontFamily = fontFamily;
            _characters = characters;
            _renderingEmSize = renderingEmSize;
            _drawingEntries = BuildDrawingEntries();
        }

        public void Draw(IDrawingContext context, Pen pen, Brush brush)
        {
            foreach (var entry in _drawingEntries)
                context.DrawGeometry(entry.Geometry, pen, brush, ref entry.Transform);
        }

        private IReadOnlyList<GlyphDrawingEntry> BuildDrawingEntries()
        {
            var geometries = new List<GlyphDrawingEntry>(_characters.Count);

            _width = 0;
            float advance = 0.0f;
            char highSurrogate = '\0';
            foreach (var character in _characters)
            {
                var isControl = char.IsControl(character);
                if (!isControl)
                {
                    uint code;
                    if (char.IsHighSurrogate(character))
                    {
                        highSurrogate = character;
                        continue;
                    }
                    else if (char.IsLowSurrogate(character))
                    {
                        Contract.Assert(highSurrogate != '\0');
                        code = (uint)char.ConvertToUtf32(highSurrogate, character);
                    }
                    else
                        code = (uint)character;

                    var glyph = _fontFamily.FindGlyph(code);
                    if (glyph != null)
                    {
                        var fontMetrics = glyph.FontMetrics;
                        var glyphMetrics = glyph.GlyphMetrics;
                        var height = (int)fontMetrics.Ascent + (int)fontMetrics.Descent;
                        var unitsPerEM = glyph.FontFace.UnitsPerEM;
                        float scale = _renderingEmSize / unitsPerEM;
                        var transform = Matrix3x2.CreateTranslation(glyphMetrics.RightSideBearing + advance, fontMetrics.Descent) *
                            Matrix3x2.CreateScale(scale, -scale) * Matrix3x2.CreateTranslation(0, height * scale);
                        advance += glyphMetrics.AdvanceWidth;
                        geometries.Add(new GlyphDrawingEntry
                        {
                            Geometry = glyph.Geometry,
                            Transform = transform
                        });

                        _width += glyphMetrics.AdvanceWidth * scale;
                        _height = Math.Max(_height, height * scale);
                        continue;
                    }
                }
            }
            return geometries;
        }

        private class GlyphDrawingEntry
        {
            public Geometry Geometry;
            public Matrix3x2 Transform;
        }
    }
}
