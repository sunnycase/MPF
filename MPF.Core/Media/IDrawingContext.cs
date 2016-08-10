using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public interface IDrawingContext : IDisposable
    {
        void DrawGeometry(Geometry geometry, Pen pen);
        void DrawGeometry(Geometry geometry, Pen pen, ref Matrix3x2 transform);
    }
}
