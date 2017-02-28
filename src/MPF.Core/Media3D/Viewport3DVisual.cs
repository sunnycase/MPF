using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace MPF.Media3D
{
    public class Viewport3DVisual : Visual
    {
        public static readonly DependencyProperty<Rect> ViewportProperty = DependencyProperty.Register(nameof(Viewport), typeof(Viewport3DVisual),
            new PropertyMetadata<Rect>(default(Rect), OnViewportPropertyChanged));
        public static readonly DependencyProperty<Camera> CameraProperty = DependencyProperty.Register(nameof(Camera), typeof(Viewport3DVisual),
            new PropertyMetadata<Camera>(new MatrixCamera(), OnCameraPropertyChanged));
        public static readonly DependencyProperty<Scene> SceneProperty = DependencyProperty.Register(nameof(Scene), typeof(Viewport3DVisual),
            new PropertyMetadata<Scene>(null, OnScenePropertyChanged));

        public Rect Viewport
        {
            get => GetValue(ViewportProperty);
            set => this.SetLocalValue(ViewportProperty, value);
        }

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

        private Rect _viewport;
        private Camera _camera;
        private Scene _scene;

        public Viewport3DVisual()
        {

        }

        internal override void RenderContent(ref SwapChainDrawingContext context)
        {
            base.RenderContent(ref context);

            var scene = _scene;
            if (scene != null)
            {
                var camera = _camera;
                var viewport = _viewport;
                viewport.Offset(_renderableObject.FinalOffset);

                if (camera != null)
                    context.PushCamera(camera);
                if (!viewport.IsEmpty)
                    context.PushViewport(ref viewport);

                foreach (var visual in scene)
                    visual.RenderContent(ref context);

                if (!viewport.IsEmpty)
                    context.PopViewport();
                if (camera != null)
                    context.PopCamera();
            }
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            var scene = _scene;
            if (scene != null)
            {
                foreach (var child in scene)
                {
                    if (child.VisualFlags.HasFlag(Visual3DFlags.RenderDirty))
                        child.Render();
                }
            }
            InvalidateRender();
        }

        private static void OnViewportPropertyChanged(object sender, PropertyChangedEventArgs<Rect> e)
        {
            ((Viewport3DVisual)sender)._viewport = e.NewValue;
        }

        private static void OnCameraPropertyChanged(object sender, PropertyChangedEventArgs<Camera> e)
        {
            ((Viewport3DVisual)sender)._camera = e.NewValue;
        }

        private static void OnScenePropertyChanged(object sender, PropertyChangedEventArgs<Scene> e)
        {
            ((Viewport3DVisual)sender)._scene = e.NewValue;
        }
    }
}
