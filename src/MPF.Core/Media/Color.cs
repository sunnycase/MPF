using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public struct Color : IEquatable<Color>
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

        public override int GetHashCode()
        {
            return A.GetHashCode() ^ R.GetHashCode() ^ G.GetHashCode() ^ B.GetHashCode();
        }

        public bool Equals(Color other)
        {
            return A == other.A && R == other.R && G == other.G && B == other.B;
        }

        public override bool Equals(object obj)
        {
            if (obj is Color)
                return Equals((Color)obj);
            return false;
        }
    }
}
