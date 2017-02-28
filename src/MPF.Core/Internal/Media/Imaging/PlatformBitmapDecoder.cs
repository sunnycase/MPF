using MPF.Interop;
using MPF.Media;
using MPF.Media.Imaging;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Collections;
using System.Runtime.CompilerServices;

namespace MPF.Internal.Media.Imaging
{
    class PlatformBitmapDecoder : BitmapDecoder
    {
        private readonly IBitmapDecoder _decoder;

        public override IReadOnlyList<BitmapFrame> Frames { get; }

        public PlatformBitmapDecoder(Stream source)
        {
            _decoder = MediaResourceManager.Current.CreateBitmapDecoderFromStream(source);
            Frames = new BitmapFrameList(_decoder);
        }

        class PlatformBitmapFrame : BitmapFrame
        {
            private readonly IBitmapFrameDecode _frame;

            public override int PixelWidth => (int)_frame.PixelSize.Width;

            public override int PixelHeight => (int)_frame.PixelSize.Height;

            public override PixelFormat PixelFormat => _frame.PixelFormat;

            public PlatformBitmapFrame(IBitmapFrameDecode frame)
            {
                _frame = frame;
            }

            public unsafe override void CopyPixels(Int32Rect? sourceRect, int stride, IntPtr buffer, int bufferLength)
            {
                Int32Rect rect;
                IntPtr rectPtr;
                if (sourceRect.HasValue)
                {
                    rect = sourceRect.Value;
                    rectPtr = new IntPtr(&rect);
                }
                else
                    rectPtr = IntPtr.Zero;

                _frame.CopyPixels(rectPtr, (uint)stride, buffer, (uint)bufferLength);
            }
        }

        class BitmapFrameList : IReadOnlyList<BitmapFrame>
        {
            private readonly IBitmapDecoder _decoder;
            private WeakConcurrentDictionary<int, BitmapFrame> _frames = new WeakConcurrentDictionary<int, BitmapFrame>();

            public BitmapFrameList(IBitmapDecoder decoder)
            {
                _decoder = decoder;
            }

            public BitmapFrame this[int index] => GetAt(index);

            public int Count => (int)_decoder.FrameCount;

            private BitmapFrame GetAt(int index)
            {
                return _frames.GetOrAdd(index, k => new PlatformBitmapFrame(_decoder.GetFrame((uint)k)));
            }

            public IEnumerator<BitmapFrame> GetEnumerator()
            {
                var count = Count;
                for (int i = 0; i < count; i++)
                    yield return GetAt(i);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return GetEnumerator();
            }
        }
    }
}
