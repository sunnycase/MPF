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

        public GlyphRun(FontFamily fontFamily, IReadOnlyList<char> characters, float renderingEmSize)
        {
            _fontFamily = fontFamily;
            _characters = characters;
            _renderingEmSize = renderingEmSize;
            _drawingEntries = BuildDrawingEntries();
        }

        private IReadOnlyList<GlyphDrawingEntry> BuildDrawingEntries()
        {
            var geometries = new List<GlyphDrawingEntry>(_characters.Count);

            char highSurrogate = '\0';
            foreach (var character in _characters)
            {
                var isBlank = char.IsControl(character) || char.IsWhiteSpace(character);
                if(!isBlank)
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
                    if(glyph != null)
                    {
                        geometries.Add(new GlyphDrawingEntry
                        {
                            Geometry = glyph.Geometry,
                            Transform = Matrix3x2.Identity
                        });
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
