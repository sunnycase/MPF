using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public struct Rect
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Width { get; set; }
        public float Height { get; set; }

        public Point Location => new Point(X, Y);
        public Size Size => new Size(Width, Height);

        public Rect(Point location, Size size)
        {
            X = location.X;
            Y = location.Y;
            Width = size.Width;
            Height = size.Height;
        }

        public Rect(float x, float y, float width, float height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }

        public static readonly Rect Zero = new Rect();
    }
}
