using MPF.Media;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace MPF.Media3D
{
    public interface IDrawingContext3D : IDisposable
    {
        void DrawGeometry3D(Geometry3D geometry, Pen pen, Brush brush);
        void DrawGeometry3D(Geometry3D geometry, Pen pen, Brush brush, ref Matrix4x4 transform);
    }
}
