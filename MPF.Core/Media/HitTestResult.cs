using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class PointHitTestResult
    {
        public Point PointHit { get; }
        public Visual VisualHit { get; }

        public PointHitTestResult(Point point, Visual visualHit)
        {
            PointHit = point;
            VisualHit = visualHit;
        }
    }

    public class PointHitTestParameters
    {
        public Point HitPoint { get; }

        public PointHitTestParameters(Point hitPoint)
        {
            HitPoint = hitPoint;
        }
    }
}
