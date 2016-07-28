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
            Point.Zero, OnStartPointPropertyChanged);

        public static readonly DependencyProperty<Point> EndPointProperty = DependencyProperty.Register(nameof(EndPoint), typeof(LineGeometry),
            Point.Zero, OnEndPointPropertyChanged);

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
            RegisterUpdateResource();
        }

        internal override IResource GetResourceOverride()
        {
            return _geometryRes;
        }

        private static void OnStartPointPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((LineGeometry)sender).RegisterUpdateResource();
        }

        bool _updateRegistered = false;
        private void RegisterUpdateResource()
        {
            if(!_updateRegistered)
            {
                _updateRegistered = true;
                DeviceContext.Current.UpdateResource += OnUpdateResource;
            }
        }

        private void OnUpdateResource(object sender, EventArgs e)
        {
            _updateRegistered = false;
            var data = new LineGeometryData
            {
                StartPoint = StartPoint,
                EndPoint = EndPoint
            };
            MediaResourceManager.Current.UpdateLineGeometry(_geometryRes, ref data);
        }

        private static void OnEndPointPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((LineGeometry)sender).RegisterUpdateResource();
        }
    }
}
