using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    public struct Point
    {
        public float X { get; set; }
        public float Y { get; set; }

        public static readonly Point Zero = default(Point);

        public Point(float x, float y)
        {
            X = x;
            Y = y;
        }

        public static implicit operator Vector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }
    }
}
