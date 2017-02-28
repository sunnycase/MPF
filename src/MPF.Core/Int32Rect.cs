using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct Int32Rect : IEquatable<Int32Rect>
    {
        public int X { get; set; }
        public int Y { get; set; }
        public int Width { get; set; }
        public int Height { get; set; }

        public bool Equals(Int32Rect other)
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }

        public override bool Equals(object obj)
        {
            if (obj is Int32Rect)
                return Equals((Int32Rect)obj);
            return false;
        }

        public override int GetHashCode()
        {
            return X.GetHashCode() ^ Y.GetHashCode() ^ Width.GetHashCode() ^ Height.GetHashCode();
        }
    }
}
