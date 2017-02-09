using System;
using System.Collections.Generic;
using System.Text;
using MPF.Interop;
using MPF.Media;
using MPF.Data;
using System.Numerics;

namespace MPF.Media3D
{
    public class BoxGeometry3D : Geometry3D
    {
        public static readonly DependencyProperty<Point3D> PositionProperty = DependencyProperty.Register(nameof(Position), typeof(BoxGeometry3D),
            new PropertyMetadata<Point3D>(Point3D.Zero, OnPositionPropertyChanged));

        public static readonly DependencyProperty<float> WidthProperty = DependencyProperty.Register(nameof(Width), typeof(BoxGeometry3D),
            new PropertyMetadata<float>(1.0f, OnWidthPropertyChanged));

        public static readonly DependencyProperty<float> HeightBottomProperty = DependencyProperty.Register(nameof(Height), typeof(BoxGeometry3D),
            new PropertyMetadata<float>(1.0f, OnHeightPropertyChanged));

        public static readonly DependencyProperty<float> DepthProperty = DependencyProperty.Register(nameof(Depth), typeof(BoxGeometry3D),
            new PropertyMetadata<float>(1.0f, OnDepthPropertyChanged));

        public Point3D Position
        {
            get => GetValue(PositionProperty);
            set => this.SetLocalValue(PositionProperty, value);
        }

        public float Width
        {
            get => GetValue(WidthProperty);
            set => this.SetLocalValue(WidthProperty, value);
        }

        public float Height
        {
            get => GetValue(HeightBottomProperty);
            set => this.SetLocalValue(HeightBottomProperty, value);
        }

        public float Depth
        {
            get => GetValue(DepthProperty);
            set => this.SetLocalValue(DepthProperty, value);
        }

        private readonly Lazy<IResource> _geometryRes;
        private BoxGeometry3DData _data;

        public BoxGeometry3D()
        {
            _geometryRes = this.CreateResource(ResourceType.RT_BoxGeometry3D);
            RegisterUpdateResource();
        }

        internal override IResource GetResourceOverride()
        {
            return _geometryRes.Value;
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdateBoxGeometry3D(_geometryRes.Value, ref _data);
        }

        private void OnPositionChanged(Point3D position)
        {
            _data.Position = position;
            RegisterUpdateResource();
        }

        private void OnWidthChanged(float width)
        {
            _data.Width = width;
            RegisterUpdateResource();
        }

        private void OnHeightChanged(float height)
        {
            _data.Height = height;
            RegisterUpdateResource();
        }

        private void OnDepthChanged(float depth)
        {
            _data.Depth = depth;
            RegisterUpdateResource();
        }

        private static void OnPositionPropertyChanged(object sender, PropertyChangedEventArgs<Point3D> e)
        {
            ((BoxGeometry3D)sender).OnPositionChanged(e.NewValue);
        }

        private static void OnWidthPropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((BoxGeometry3D)sender).OnWidthChanged(e.NewValue);
        }

        private static void OnHeightPropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((BoxGeometry3D)sender).OnHeightChanged(e.NewValue);
        }

        private static void OnDepthPropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((BoxGeometry3D)sender).OnDepthChanged(e.NewValue);
        }
    }
}
