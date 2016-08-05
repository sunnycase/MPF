using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    public struct Size
    {
        public float Width { get; set; }
        public float Height { get; set; }

        public static readonly Size Zero = default(Size);

        public Size(float width, float height)
        {
            Width = width;
            Height = height;
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
