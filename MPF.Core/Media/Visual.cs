using MPF.Data;
using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    [Flags]
    internal enum VisualFlags : uint
    {
        None = 0x0,
        BBoxDirty = 0x1
    }

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

        internal VisualFlags VisualFlags { get; private set; } = VisualFlags.BBoxDirty;

        internal bool IsVisualVisible { get; set; } = true;

        public virtual int VisualChildrenCount => 0;

        private Vector2 _visualOffset;
        private RenderData _renderData;
        internal readonly IRenderableObject _renderableObject;

        private Visual _parent;
        internal Visual Parent
        {
            get { return _parent; }
            private set
            {
                if(_parent != value)
                {
                    _parent = value;
                    _renderableObject.SetParent(_parent?._renderableObject);
                    OnParentChanged(value);
                }
            }
        }

        internal event EventHandler ParentChanged;

        private int _visualLevel = 0;
        internal int VisualLevel
        {
            get { return _visualLevel; }
            private set
            {
                if (_visualLevel != value)
                {
                    _visualLevel = value;
                    foreach (var child in VisualChildren)
                        child.VisualLevel = value + 1;
                }
            }
        }

        private Rect _bboxSubgraph;

        internal Visual()
        {
            _renderableObject = DeviceContext.Current.CreateRenderableObject();
        }

        internal void HitTest(PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            Precompute();
            HitTestPoint(param, filter, resultCallback);
        }

        public PointHitTestResult HitTest(Point point)
        {
            var result = new TopMostHitResult();
            VisualTreeHelper.HitTest(this, new PointHitTestParameters(point), null, result.HitTestResult);
            return result.HitResult;
        }

        class TopMostHitResult
        {
            public PointHitTestResult HitResult { get; private set; }

            internal HitTestResultBehavior HitTestResult(PointHitTestResult result)
            {
                HitResult = result;
                return HitTestResultBehavior.Stop;
            }

            internal HitTestFilterBehavior NoNested2DFilter(DependencyObject potentialHitTestTarget)
            {
                //if (potentialHitTestTarget is Viewport2DVisual3D)
                //{
                //    return HitTestFilterBehavior.ContinueSkipChildren;
                //}
                return HitTestFilterBehavior.Continue;
            }
        }

        private HitTestResultBehavior HitTestPoint(PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            if (_bboxSubgraph.Contains(param.HitPoint))
            {
                var hitTestFilterBehavior = HitTestFilterBehavior.Continue;
                if (filter != null)
                {
                    hitTestFilterBehavior = filter(this);
                    if (hitTestFilterBehavior == HitTestFilterBehavior.ContinueSkipSelfAndChildren)
                        return HitTestResultBehavior.Continue;
                    if (hitTestFilterBehavior == HitTestFilterBehavior.Stop)
                        return HitTestResultBehavior.Stop;
                }
                var hitPoint = param.HitPoint;
                var point = hitPoint;

                if (hitTestFilterBehavior != HitTestFilterBehavior.ContinueSkipChildren)
                {
                    foreach (var child in VisualChildren)
                    {
                        var childPoint = point - child.VisualOffset;
                        param.SetHitPoint(childPoint);
                        var hitTestResultBehavior = child.HitTestPoint(param, filter, resultCallback);
                        param.SetHitPoint(hitPoint);
                        if (hitTestResultBehavior == HitTestResultBehavior.Stop)
                            return HitTestResultBehavior.Stop;
                    }
                }

                if (hitTestFilterBehavior != HitTestFilterBehavior.ContinueSkipSelf)
                {
                    param.SetHitPoint(point);
                    var hitTestResultBehavior = this.HitTestOverride(param, filter, resultCallback);
                    param.SetHitPoint(hitPoint);
                    if (hitTestResultBehavior == HitTestResultBehavior.Stop)
                        return HitTestResultBehavior.Stop;
                }
            }
            return HitTestResultBehavior.Continue;
        }

        protected virtual HitTestResultBehavior HitTestOverride(PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            var hitTestResult = HitTestCore(param);
            if (hitTestResult != null)
                return resultCallback(hitTestResult);
            return HitTestResultBehavior.Continue;
        }

        protected virtual PointHitTestResult HitTestCore(PointHitTestParameters param)
        {
            if (GetHitTestBounds().Contains(param.HitPoint))
                return new PointHitTestResult(param.HitPoint, this);
            return null;
        }

        private void Precompute()
        {
            if (VisualFlags.HasFlag(VisualFlags.BBoxDirty))
            {
                Rect rect;
                PrecomputeRecursive(out rect);
            }
        }

        protected void InvalidateBoundingBox()
        {
            VisualFlags |= VisualFlags.BBoxDirty;
        }

        protected virtual void PrecomputeRecursive(out Rect bboxSubgraph)
        {
            if (VisualFlags.HasFlag(VisualFlags.BBoxDirty))
            {
                PrecomputeContent();
                foreach (var child in VisualChildren)
                {
                    Rect rect;
                    child.PrecomputeRecursive(out rect);
                    _bboxSubgraph = Rect.Union(_bboxSubgraph, rect);
                }
                VisualFlags &= ~VisualFlags.BBoxDirty;
            }
            bboxSubgraph = _bboxSubgraph;
        }

        protected void AddVisualChild(Visual visual)
        {
            if (visual.Parent != null)
                throw new InvalidOperationException("Visual already has a parent.");
            visual.Parent = this;
            visual.VisualLevel = VisualLevel + 1;
            InvalidateBoundingBox();
        }

        protected void RemoveVisualChild(Visual visual)
        {
            if (visual.Parent != this)
                throw new InvalidOperationException("Visual's parent is not this.");
            visual.Parent = null;
            visual.VisualLevel = 0;
            InvalidateBoundingBox();
        }

        protected virtual void OnParentChanged(Visual visual)
        {
            ParentChanged?.Invoke(this, EventArgs.Empty);
        }

        protected virtual PointHitTestResult HitTestOverride(PointHitTestParameters param)
        {
            return null;
        }

        public virtual Visual GetVisualChildAt(int index)
        {
            throw new ArgumentOutOfRangeException(nameof(index));
        }

        protected virtual Rect GetContentBounds()
        {
            return new Rect(Point.Zero, Size.Zero);
        }

        protected virtual Rect GetHitTestBounds()
        {
            return GetContentBounds();
        }

        protected virtual void PrecomputeContent()
        {
            _bboxSubgraph = GetHitTestBounds();
            if (float.IsNaN(_bboxSubgraph.Left) || float.IsNaN(_bboxSubgraph.Top) ||
                float.IsNaN(_bboxSubgraph.Width) || float.IsNaN(_bboxSubgraph.Height))
            {
                _bboxSubgraph = new Rect(float.NegativeInfinity, float.NegativeInfinity, float.PositiveInfinity, float.PositiveInfinity);
            }
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
            foreach (var child in VisualChildren)
                child.UpdateVisualOffset(child.VisualOffset);
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
