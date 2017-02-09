using MPF.Media3D;
using System;
using System.Collections.Generic;
using System.Text;
using MPF.Media;
using MPF.Data;

namespace MPF.Controls
{
    public class Viewport3D : FrameworkElement
    {
        public static readonly DependencyProperty<Camera> CameraProperty = Viewport3DVisual.CameraProperty.AddOwner(typeof(Viewport3D),
            new PropertyMetadata<Camera>(DependencyProperty.UnsetValue, OnCameraPropertyChanged));
        public static readonly DependencyProperty<Scene> SceneProperty = Viewport3DVisual.SceneProperty.AddOwner(typeof(Viewport3D),
            new PropertyMetadata<Scene>(DependencyProperty.UnsetValue, OnScenePropertyChanged));

        public Camera Camera
        {
            get => GetValue(CameraProperty);
            set => this.SetLocalValue(CameraProperty, value);
        }

        public Scene Scene
        {
            get => GetValue(SceneProperty);
            set => this.SetLocalValue(SceneProperty, value);
        }

        private readonly Viewport3DVisual _viewportVisual = new Viewport3DVisual();

        public override int VisualChildrenCount => 1;

        public Viewport3D()
        {
            AddVisualChild(_viewportVisual);
        }

        public override Visual GetVisualChildAt(int index)
        {
            if (index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));

            return _viewportVisual;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            _viewportVisual.Viewport = new Rect(Point.Zero, finalSize);
            return finalSize;
        }

        private static void OnCameraPropertyChanged(object sender, PropertyChangedEventArgs<Camera> e)
        {
            ((Viewport3D)sender)._viewportVisual.Camera = e.NewValue;
        }

        private static void OnScenePropertyChanged(object sender, PropertyChangedEventArgs<Scene> e)
        {
            ((Viewport3D)sender)._viewportVisual.Scene = e.NewValue;
        }
    }
}
