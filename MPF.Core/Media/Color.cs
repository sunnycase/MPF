using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public struct Color
    {
        public byte A { get; set; }
        public byte R { get; set; }
        public byte G { get; set; }
        public byte B { get; set; }

        internal ColorF ToColorF()
        {
            return new ColorF
            {
                A = A / 255.0f,
                R = R / 255.0f,
                G = G / 255.0f,
                B = B / 255.0f
            };
        }

        public static Color FromArgb(uint argb)
        {
            return new Color
            {
                A = (byte)(argb >> 24),
                R = (byte)(argb >> 16),
                G = (byte)(argb >> 8),
                B = (byte)(argb)
            };
        }
    }
}
