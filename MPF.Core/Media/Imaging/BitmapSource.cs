using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media.Imaging
{
    public sealed class BitmapSource : ImageSource
    {
        private readonly Lazy<IResource> _bitmapSrcRes;

        public BitmapSource()
        {

        }

        internal override IResource GetResourceOverride() => _bitmapSrcRes.Value;
    }
}
