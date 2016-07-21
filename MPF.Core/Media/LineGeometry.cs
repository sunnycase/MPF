using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class LineGeometry : Geometry
    {
        public static readonly DependencyProperty<Point> StartPointProperty = DependencyProperty.Register(nameof(StartPoint), typeof(LineGeometry),
            Point.Zero);

        public static readonly DependencyProperty<Point> EndPointProperty = DependencyProperty.Register(nameof(EndPoint), typeof(LineGeometry),
            Point.Zero);

        public Point StartPoint
        {
            get { return GetValue(StartPointProperty); }
            set { SetValue(StartPointProperty, value); }
        }

        public Point EndPoint
        {
            get { return GetValue(EndPointProperty); }
            set { SetValue(EndPointProperty, value); }
        }

        private readonly IResource _geometryRes;

        public LineGeometry()
        {
            _geometryRes = MediaResourceManager.Current.CreateResouce(ResourceType.RT_LineGeometry);
        }
    }
}
