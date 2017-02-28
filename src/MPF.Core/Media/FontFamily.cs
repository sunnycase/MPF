using MPF.Internal.FontCache;
using MPF.Internal.Text;
using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class FontFamily
    {
        private FontFamilyIdentifier _familyIdentifier;
        private IFontFamily _firstFontFamily;
        internal IFontFamily FirstFontFamily
        {
            get
            {
                if (_firstFontFamily == null)
                    _firstFontFamily = LookupFirstFontFamily();
                return _firstFontFamily;
            }
        }

        public Uri BaseUri => _familyIdentifier.BaseUri;

        public FontFamily(Uri baseUri, string familyName)
        {
            _familyIdentifier = new FontFamilyIdentifier(familyName, baseUri);
        }

        public FontFamily(string familyName)
            :this(null, familyName)
        {

        }

        internal GlyphFace FindGlyph(uint code)
        {
            GlyphFace glyph = null;
            for (int i = 0; i < _familyIdentifier.TokenCount; i++)
            {
                try
                {
                    var fontFace = LookupFontFamily(_familyIdentifier[i]);
                    if (fontFace?.TryFindGlyph(code, out glyph) ?? false)
                        break;
                }
                catch { }
            }
            return glyph;
        }

        private IFontFamily LookupFirstFontFamily()
        {
            for (int i = 0; i < _familyIdentifier.TokenCount; i++)
            {
                try
                {
                    var fontFace = LookupFontFamily(_familyIdentifier[i]);
                    if (fontFace == null) continue;
                    return fontFace;
                }
                catch { }
            }
            throw new InvalidOperationException("No font family can be found.");
        }

        private static IFontFamily LookupFontFamily(NormalizedFontFamilyReference familyReference)
        {
            if (familyReference.IsUnresolved)
                return null;

            FamilyCollection familyCollection;
            if (familyReference.LocationUri != null)
                familyCollection = FamilyCollection.FromUri(familyReference.LocationUri);
            else
                familyCollection = FamilyCollection.Default;
            return familyCollection.LookupFamily(familyReference.FamilyName);
        }
    }
}
