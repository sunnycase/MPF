using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    [DebuggerDisplay("Location = {Location}, Size = {Size}")]
    public struct Rect
    {
        public float Left { get; set; }
        public float Top { get; set; }
        public float Width { get; set; }
        public float Height { get; set; }
        public float Right => Left + Width;
        public float Bottom => Top + Height;
        public bool IsEmpty => Left == 0 && Top == 0 && Width == 0 && Height == 0;

        public Point Location => new Point(Left, Top);
        public Size Size => new Size(Width, Height);

        public Rect(Point location, Size size)
        {
            Left = location.X;
            Top = location.Y;
            Width = size.Width;
            Height = size.Height;
        }

        public Rect(float left, float top, float width, float height)
        {
            Left = left;
            Top = top;
            Width = width;
            Height = height;
        }

        public bool Contains(Point point)
        {
            return Left <= point.X && Top <= point.Y && Right >= point.X && Bottom >= point.Y;
        }

        public static readonly Rect Zero = new Rect();

        public static Rect Union(Rect a, Rect b)
        {
            if (a.IsEmpty) return b;
            if (b.IsEmpty) return a;

            var left = Math.Min(a.Left, b.Left);
            var top = Math.Min(a.Top, b.Top);
            float width, height;

            if (a.Width == float.PositiveInfinity || b.Width == float.PositiveInfinity)
                width = float.PositiveInfinity;
            else
            {
                float right = Math.Max(a.Right, b.Right);
                width = Math.Max(right - left, 0.0f);
            }

            if (a.Height == float.PositiveInfinity || b.Height == float.PositiveInfinity)
                height = float.PositiveInfinity;
            else
            {
                float bottom = Math.Max(a.Bottom, b.Bottom);
                height = Math.Max(bottom - top, 0.0f);
            }
            return new Rect(left, top, width, height);
        }
    }
}
