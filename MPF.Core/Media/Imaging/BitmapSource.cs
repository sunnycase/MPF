using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Media.Imaging
{
    public abstract class BitmapSource : ImageSource
    {
        public abstract int PixelWidth { get; }
        public abstract int PixelHeight { get; }
        public abstract PixelFormat PixelFormat { get; }

        private Lazy<IResource> _textureRes;

        public BitmapSource()
        {
        }

        public abstract void CopyPixels(Int32Rect? sourceRect, int stride, IntPtr buffer, int bufferLength);

        internal override IResource GetTextureResourceOverride()
        {
            if (_textureRes == null)
            {
                _textureRes = this.CreateResource(ResourceType.RT_MemoryTexture);
                RegisterUpdateResource();
            }
            return _textureRes.Value;
        }

        internal unsafe override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);

            var format = PixelFormat;
            var bytesPerPixel = (format.GetBitsPerPixel() + 7) / 8;
            var width = (uint)PixelWidth;
            var height = (uint)PixelHeight;
            var rowPitch = 4 * ((width * bytesPerPixel + 3) / 4);
            var pixels = new byte[rowPitch * height];
            fixed (byte* pixelsPtr = pixels)
            {
                var pixelsIPtr = new IntPtr(pixelsPtr);
                CopyPixels(null, (int)rowPitch, pixelsIPtr, pixels.Length);
                var data = new MemoryTextureData
                {
                    Format = format,
                    Dimension = 2,
                    Width = width,
                    Height = height,
                    RowPitch = rowPitch,
                    Pixels = pixelsIPtr,
                    PixelsLength = (uint)pixels.Length
                };
                MediaResourceManager.Current.UpdateMemoryTexture(_textureRes.Value, ref data);
            }
        }
    }
}
