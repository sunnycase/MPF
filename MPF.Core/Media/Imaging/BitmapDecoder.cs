using MPF.Internal.Media.Imaging;
using MPF.Threading;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace MPF.Media.Imaging
{
    public abstract class BitmapDecoder : DispatcherObject
    {
        public abstract IReadOnlyList<BitmapFrame> Frames { get; }

        public static BitmapDecoder Create(Stream source)
        {
            return new PlatformBitmapDecoder(source);
        }
    }
}
