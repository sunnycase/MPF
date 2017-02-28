using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace MPF.Media3D
{
    public struct Point3D
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }

        public static readonly Point3D Zero = default(Point3D);

        public Point3D(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public override string ToString()
        {
            return $"{X}, {Y}, {Z}";
        }

        public static implicit operator Point3D((float x, float y, float z) value)
        {
            return new Point3D(value.x, value.y, value.z);
        }

        public static explicit operator Vector3(Point3D point)
        {
            return new Vector3(point.X, point.Y, point.Z);
        }

        public static explicit operator Point3D(Vector3 vector)
        {
            return new Point3D(vector.X, vector.Y, vector.Z);
        }

        public static Point3D operator +(Point3D left, Vector3 right)
        {
            return new Point3D(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
        }

        public static Point3D operator -(Point3D left, Vector3 right)
        {
            return new Point3D(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
        }
    }
}
