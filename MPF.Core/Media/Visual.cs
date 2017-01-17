using MPF.Data;
using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public abstract class Visual : DependencyObject
    {
        public static readonly DependencyProperty<bool> IsHitTestVisibleProperty = DependencyProperty.Register(nameof(IsHitTestVisible),
            typeof(Visual), new PropertyMetadata<bool>(true));

        private static readonly DependencyProperty<Geometry> VisualClipProperty = DependencyProperty.Register(nameof(VisualClip),
            typeof(Visual), new PropertyMetadata<Geometry>(Geometry.Empty));

        public bool IsHitTestVisible
        {
            get { return GetValue(IsHitTestVisibleProperty); }
            set { this.SetLocalValue(IsHitTestVisibleProperty, value); }
        }

        public Geometry VisualClip
        {
            get { return GetValue(VisualClipProperty); }
            set { this.SetLocalValue(VisualClipProperty, value); }
        }

        public Vector2 VisualOffset
        {
            get { return _visualOffset; }
            protected set
            {
                if (_visualOffset != value)
                {
                    _visualOffset = value;
                    UpdateVisualOffset(value);
                }
            }
        }

        internal bool IsVisualVisible { get; set; } = true;

        public virtual int VisualChildrenCount => 0;

        private Vector2 _visualOffset;
        private RenderData _renderData;
        internal readonly IRenderableObject _renderableObject;

        private Visual _parent;
        internal Visual Parent => _parent;

        private int _visualLevel = 0;
        internal int VisualLevel
        {
            get { return _visualLevel; }
            private set
            {
                if(_visualLevel != value)
                {
                    _visualLevel = value;
                    foreach (var child in VisualChildren)
                        child.VisualLevel = value + 1;
                }
            }
        }

        internal Visual()
        {
            _renderableObject = DeviceContext.Current.CreateRenderableObject();
        }

        public void HitTest(PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            if (!IsHitTestVisible) return;

        }

        protected void AddVisualChild(Visual visual)
        {
            if (visual._parent != null)
                throw new InvalidOperationException("Visual already has a parent.");
            visual._parent = this;
            visual.VisualLevel = VisualLevel + 1;
        }

        protected void RemoveVisualChild(Visual visual)
        {
            if (visual._parent != this)
                throw new InvalidOperationException("Visual's parent is not this.");
            visual._parent = null;
            visual.VisualLevel = 0;
        }

        protected virtual PointHitTestResult HitTestOverride(PointHitTestParameters param)
        {
            return null;
        }

        public virtual Visual GetVisualChildAt(int index)
        {
            throw new ArgumentOutOfRangeException(nameof(index));
        }

        public virtual Rect GetContentBounds()
        {
            return Rect.Zero;
        }

        public IEnumerable<Visual> VisualChildren
        {
            get
            {
                var count = VisualChildrenCount;
                for (int i = 0; i < count; i++)
                    yield return GetVisualChildAt(i);
            }
        }

        internal void Render()
        {
            using (var drawingContext = RenderOpen())
            {
                OnRender(drawingContext);
            }
        }

        private IDrawingContext RenderOpen()
        {
            return new VisualDrawingContext(this);
        }

        private void RenderClose(RenderData renderData)
        {
            _renderData = renderData;
            _renderableObject.SetContent(_renderData.Close());
        }

        protected virtual void OnRender(IDrawingContext drawingContext)
        {
        }

        internal void RenderContent()
        {
            _renderableObject.Render();
        }

        private void UpdateVisualOffset(Vector2 value)
        {
            _renderableObject.SetOffset(value.X, value.Y);
        }

        class VisualDrawingContext : RenderDataDrawingContext
        {
            private readonly Visual _visual;
            public VisualDrawingContext(Visual visual)
            {
                _visual = visual;
            }

            protected override void CloseOverride(RenderData renderData)
            {
                _visual.RenderClose(renderData);
                base.CloseOverride(renderData);
            }
        }
    }

    public enum HitTestFilterBehavior
    {
        ContinueSkipChildren,
        ContinueSkipSelfAndChildren,
        ContinueSkipSelf,
        Continue,
        Stop
    }

    public enum HitTestResultBehavior
    {
        Stop,
        Continue
    }

    public delegate HitTestFilterBehavior HitTestFilterCallback<in T>(T obj);
    public delegate HitTestResultBehavior HitTestResultCallback<in T>(T result);
}
