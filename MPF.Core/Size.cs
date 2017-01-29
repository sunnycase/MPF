using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    public struct Size : IEquatable<Size>
    {
        public float Width { get; set; }
        public float Height { get; set; }

        public static readonly Size Zero = default(Size);

        public Size(float width, float height)
        {
            Width = width;
            Height = height;
        }

        public override string ToString()
        {
            return $"{Width}, {Height}";
        }

        public bool Equals(Size other)
        {
            return Width == other.Width && Height == other.Height;
        }

        public override bool Equals(object obj)
        {
            if (obj is Size)
                return Equals((Size)obj);
            return false;
        }

        public override int GetHashCode()
        {
            return Width.GetHashCode() ^ Height.GetHashCode();
        }

        public static bool operator==(Size left, Size right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(Size left, Size right)
        {
            return !left.Equals(right);
        }

        public static explicit operator Point(Size size)
        {
            return new Point(size.Width, size.Height);
        }

        public static explicit operator Vector2(Size size)
        {
            return new Vector2(size.Width, size.Height);
        }
    }
}
