using MPF.Media.Imaging;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Interop
{
    [ComImport]
    [Guid("8705DFC6-752C-4C28-A51B-B156CFF3AB50")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IBitmapDecoder
    {
        uint FrameCount { get; }
        IBitmapFrameDecode GetFrame(uint id);
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    struct UInt32Size
    {
        public uint Width, Height;
    }

    [ComImport]
    [Guid("B12842B0-0313-4199-A24E-745E840D2B74")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IBitmapFrameDecode
    {
        UInt32Size PixelSize { get; }
        PixelFormat PixelFormat { get; }
        void CopyPixels([In]IntPtr srcRect, uint stride, IntPtr buffer, uint bufferSize);
    }
}
