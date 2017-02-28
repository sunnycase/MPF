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

        public override string ToString()
        {
            return $"{X}, {Y}";
        }

        public static explicit operator Vector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }

        public static explicit operator Point(Vector2 vector)
        {
            return new Point(vector.X, vector.Y);
        }

        public static Point operator +(Point left, Vector2 right)
        {
            return new Point(left.X + right.X, left.Y + right.Y);
        }

        public static Point operator -(Point left, Vector2 right)
        {
            return new Point(left.X - right.X, left.Y - right.Y);
        }
    }
}
