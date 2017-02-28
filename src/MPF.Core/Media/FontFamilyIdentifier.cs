using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal struct FontFamilyIdentifier
    {
        public string FriendlyName { get; }
        public Uri BaseUri { get; }

        private int _tokenCount;
        public int TokenCount
        {
            get
            {
                Normalize();
                return _tokenCount;
            }
        }

        private NormalizedFontFamilyReference[] _nffReferences;
        public NormalizedFontFamilyReference this[int index]
        {
            get
            {
                Normalize();
                return _nffReferences[index];
            }
        }

        public FontFamilyIdentifier(string friendlyName, Uri baseUri)
        {
            FriendlyName = friendlyName;
            BaseUri = baseUri;
            _tokenCount = -1;
            _nffReferences = null;
        }

        public void Normalize()
        {
            if (_tokenCount != -1) return;
            var tokens = FriendlyName.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
            var count = tokens.Length;
            var array = new NormalizedFontFamilyReference[count];
            for (int i = 0; i < count; i++)
                array[i] = NormalizedFontFamilyReference.Create(BaseUri, NormalizeFontFamilyReference(tokens[i]));
            _tokenCount = count;
            _nffReferences = array;
        }

        private string NormalizeFontFamilyReference(string token)
        {
            if (token.StartsWith("#"))
                return token.ToUpperInvariant();
            return "#" + token.ToUpperInvariant();
        }
    }

    internal class NormalizedFontFamilyReference
    {
        private static readonly NormalizedFontFamilyReference _unresolved = new NormalizedFontFamilyReference((string)null, string.Empty);

        public Uri LocationUri { get; }
        public string FamilyName { get; }
        public string EscapedFileName { get; set; }

        public bool IsUnresolved => this == _unresolved;

        private NormalizedFontFamilyReference(Uri locationUri, string familyName)
        {
            LocationUri = locationUri;
            FamilyName = familyName;
        }

        private NormalizedFontFamilyReference(string escapedFileName, string familyName)
        {
            EscapedFileName = escapedFileName;
            FamilyName = familyName;
        }

        public static NormalizedFontFamilyReference Create(Uri baseUri, string normalizedReference)
        {
            string location, escapedFamilyName;
            SplitLocationAndEscapedFamilyName(normalizedReference, out location, out escapedFamilyName);
            bool resolved = false;
            Uri uri = null;
            if (string.IsNullOrEmpty(location))
                resolved = true;
            else
            {
                if (Uri.TryCreate(location, UriKind.Absolute, out uri))
                    resolved = true;
                else if (Uri.TryCreate(baseUri, location, out uri))
                    resolved = true;
            }
            if(resolved)
            {
                var familyName = Uri.UnescapeDataString(escapedFamilyName);
                if (uri != null)
                    return new NormalizedFontFamilyReference(uri, familyName);
                else
                    return new NormalizedFontFamilyReference(location, familyName);
            }
            return _unresolved;
        }

        private static void SplitLocationAndEscapedFamilyName(string normalizedReference, out string location, out string escapedFamilyName)
        {
            var tokens = normalizedReference.Split(new char[] { '#' }, 2, StringSplitOptions.RemoveEmptyEntries);
            if(tokens.Length == 0)
            {
                location = null;
                escapedFamilyName = string.Empty;
            }
            else if(tokens.Length == 1)
            {
                location = null;
                escapedFamilyName = tokens[0];
            }
            else
            {
                location = tokens[0];
                escapedFamilyName = tokens[1];
            }
        }
    }
}
