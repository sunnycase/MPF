using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media.Imaging
{
    public enum PixelFormat
    {
        PixelFormatUnknown,
        PixelFormat1bppIndexed,
        PixelFormat2bppIndexed,
        PixelFormat4bppIndexed,
        PixelFormat8bppIndexed,
        PixelFormatBlackWhite,
        PixelFormat2bppGray,
        PixelFormat4bppGray,
        PixelFormat8bppGray,
        PixelFormat8bppAlpha,
        PixelFormat16bppBGR555,
        PixelFormat16bppBGR565,
        PixelFormat16bppGray,
        PixelFormat24bppBGR,
        PixelFormat24bppRGB,
        PixelFormat32bppBGR,
        PixelFormat32bppBGRA,
        PixelFormat32bppPBGRA,
        PixelFormat32bppGrayFloat,
        PixelFormat32bppRGB,
        PixelFormat32bppRGBA,
        PixelFormat32bppPRGBA,
        PixelFormat48bppRGB,
        PixelFormat48bppBGR,
        PixelFormat64bppRGB,
    }
}
