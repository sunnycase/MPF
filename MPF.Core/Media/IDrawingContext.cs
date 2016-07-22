using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public interface IDrawingContext : IDisposable
    {
        void DrawGeometry(Geometry geometry);
    }
}
