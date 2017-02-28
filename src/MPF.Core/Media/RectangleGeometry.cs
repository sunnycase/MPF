using MPF.Data;
using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class RectangleGeometry : Geometry
    {
        public static readonly DependencyProperty<Point> LeftTopProperty = DependencyProperty.Register(nameof(LeftTop), typeof(RectangleGeometry),
            new PropertyMetadata<Point>(Point.Zero, OnLeftTopPropertyChanged));

        public static readonly DependencyProperty<Point> RigthBottomProperty = DependencyProperty.Register(nameof(RigthBottom), typeof(RectangleGeometry),
            new PropertyMetadata<Point>(Point.Zero, OnRigthBottomPropertyChanged));

        public Point LeftTop
        {
            get { return GetValue(LeftTopProperty); }
            set { this.SetLocalValue(LeftTopProperty, value); }
        }

        public Point RigthBottom
        {
            get { return GetValue(RigthBottomProperty); }
            set { this.SetLocalValue(RigthBottomProperty, value); }
        }

        private RectangleGeometryData _data;

        private readonly Lazy<IResource> _geometryRes;

        public RectangleGeometry()
        {
            _geometryRes = this.CreateResource(ResourceType.RT_RectangleGeometry);
            OnLeftTopChanged(LeftTop);
            OnRightBottomChanged(RigthBottom);
        }

        internal override IResource GetResourceOverride()
        {
            return _geometryRes.Value;
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdateRectangleGeometry(_geometryRes.Value, ref _data);
        }

        private void OnRightBottomChanged(Point rigthBottom)
        {
            _data.RightBottom = rigthBottom;
            RegisterUpdateResource();
        }

        private void OnLeftTopChanged(Point leftTop)
        {
            _data.LeftTop = leftTop;
            RegisterUpdateResource();
        }

        private static void OnLeftTopPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((RectangleGeometry)sender).OnLeftTopChanged(e.NewValue);
        }

        private static void OnRigthBottomPropertyChanged(object sender, PropertyChangedEventArgs<Point> e)
        {
            ((RectangleGeometry)sender).OnRightBottomChanged(e.NewValue);
        }
    }
}
