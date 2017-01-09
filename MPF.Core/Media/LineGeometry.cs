using MPF.Data;
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
            new PropertyMetadata<Point>(Point.Zero, OnStartPointPropertyChanged));

        public static readonly DependencyProperty<Point> EndPointProperty = DependencyProperty.Register(nameof(EndPoint), typeof(LineGeometry),
            new PropertyMetadata<Point>(Point.Zero, OnEndPointPropertyChanged));

        public Point StartPoint
        {
            get { return GetValue(StartPointProperty); }
            set { this.SetLocalValue(StartPointProperty, value); }
        }

        public Point EndPoint
        {
            get { return GetValue(EndPointProperty); }
            set { this.SetLocalValue(EndPointProperty, value); }
        }

        private readonly Lazy<IResource> _geometryRes;

        public LineGeometry()
        {
            _geometryRes = this.CreateResource(ResourceType.RT_LineGeometry);
            RegisterUpdateResource();
        }

        internal override IResource GetResourceOverride()
        {
            return _geometryRes.Value;
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            var data = new LineGeometryData
            {
                StartPoint = StartPoint,
                EndPoint = EndPoint
            };
            MediaResourceManager.Current.UpdateLineGeometry(_geometryRes.Value, ref data);
        }

        private static void OnEndPointPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((LineGeometry)sender).RegisterUpdateResource();
        }

        private static void OnStartPointPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((LineGeometry)sender).RegisterUpdateResource();
        }
    }
}
