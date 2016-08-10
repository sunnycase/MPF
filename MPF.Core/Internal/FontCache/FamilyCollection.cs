using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.IO.MemoryMappedFiles;
using System.IO;
using MPF.Media;
using System.Runtime.InteropServices;
using CodeProject.ObjectPool;
using MPF.Internal.Text;

namespace MPF.Internal.FontCache
{
    internal class FamilyCollection
    {
        private static readonly ParameterizedObjectPool<Uri, PooledObjectWrapper<FamilyCollection>> _familyCollectionCache;
        public static FamilyCollection Default { get; } = new FamilyCollection();

        private readonly Uri _locationUri;
        private readonly ParameterizedObjectPool<string, PooledObjectWrapper<IFontFamily>> _familyCache;

        static FamilyCollection()
        {
            _familyCollectionCache = new ParameterizedObjectPool<Uri, PooledObjectWrapper<FamilyCollection>>(5, 64, uri =>
                new PooledObjectWrapper<FamilyCollection>(new FamilyCollection(uri)));
        }

        private FamilyCollection()
        {
            _familyCache = new ParameterizedObjectPool<string, PooledObjectWrapper<IFontFamily>>(5, 64, location =>
                new PooledObjectWrapper<IFontFamily>(new FileFontFamily(location)));
        }

        private FamilyCollection(Uri locationUri)
            :this()
        {
            _locationUri = locationUri;
        }

        public static FamilyCollection FromUri(Uri locationUri)
        {
            return _familyCollectionCache.GetObject(locationUri).InternalResource;
        }

        public IFontFamily LookupFamily(string familyName)
        {
            Uri locationUri = _locationUri ?? Platform.GetSystemFontFaceLocation(familyName);
            return LookupFamily(locationUri);
        }

        private IFontFamily LookupFamily(Uri locationUri)
        {
            if (!locationUri.IsFile)
                throw new NotSupportedException("Uri scheme is not supported.");

            return _familyCache.GetObject(locationUri.LocalPath).InternalResource;
        }
    }

    internal interface IFontFamily
    {
        IReadOnlyCollection<FontFace> FontFaces { get; }
        bool TryFindGlyph(uint code, out GlyphFace glyph);
    }

    internal class FileFontFamily : IFontFamily, IDisposable
    {
        private readonly MemoryMappedFile _file;
        private readonly MemoryMappedViewAccessor _accessor;
        private readonly SafeBuffer _buffer;

        private IReadOnlyCollection<FontFace> _fontFaces;
        public IReadOnlyCollection<FontFace> FontFaces
        {
            get
            {
                if (_fontFaces == null)
                    _fontFaces = CreateFontFaces();
                return _fontFaces;
            }
        }

        public FileFontFamily(string location)
        {
            var stream = new FileStream(location, FileMode.Open, FileAccess.Read, FileShare.Read, 4096, 0);
            _file = MemoryMappedFile.CreateFromFile(stream, null, 0, MemoryMappedFileAccess.Read, HandleInheritability.None, false);
            _accessor = _file.CreateViewAccessor(0, 0, MemoryMappedFileAccess.Read);
            _buffer = _accessor.SafeMemoryMappedViewHandle;
        }

        public bool TryFindGlyph(uint code, out GlyphFace glyph)
        {
            foreach(var face in FontFaces)
            {
                var theGlyph = FindGlyph(face, code);
                if(theGlyph != null)
                {
                    glyph = theGlyph;
                    return true;
                }
            }
            glyph = null;
            return false;
        }

        private GlyphFace FindGlyph(FontFace face, uint code)
        {
            try
            {
                return face.GetGlyph(code);
            }
            catch
            {
                return null;
            }
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    _buffer?.Dispose();
                    _accessor?.Dispose();
                    _file?.Dispose();
                }
                disposedValue = true;
            }
        }
        
        public void Dispose()
        {
            Dispose(true);
        }
        
        private IReadOnlyCollection<FontFace> CreateFontFaces()
        {
            var first = MediaResourceManager.Current.CreateFontFaceFromMemory(_buffer.DangerousGetHandle(), _buffer.ByteLength, 0);
            var faceCount = first.FaceCount;
            var faces = new List<FontFace>((int)faceCount) { new FontFace(this, first) };
            for (uint i = 1; i < faceCount; i++)
                faces.Add(new FontFace(this, MediaResourceManager.Current.CreateFontFaceFromMemory(_buffer.DangerousGetHandle(), _buffer.ByteLength, i)));
            return faces;
        }
        #endregion
    }
}
