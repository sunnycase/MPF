using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public interface IStreamGeometryContext : IDisposable
    {
        void MoveTo(Point point);
        void LineTo(Point point);
        void ArcTo(Point point, float angle);
    }
}
