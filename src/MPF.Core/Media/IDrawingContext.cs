using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public interface IDrawingContext : IDisposable
    {
        void DrawGeometry(Geometry geometry, Pen pen, Brush brush);
        void DrawGeometry(Geometry geometry, Pen pen, Brush brush, ref Matrix3x2 transform);
    }
}
