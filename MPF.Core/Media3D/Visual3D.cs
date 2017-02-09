using System;
using System.Collections.Generic;
using System.Text;
using MPF.Media;
using MPF.Interop;
using System.Numerics;

namespace MPF.Media3D
{
    [Flags]
    internal enum Visual3DFlags : uint
    {
        None = 0x0,
        BBoxDirty = 0x1,
        RenderDirty = 0x2
    }

    public class Visual3D : DependencyObject
    {
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

        internal Visual3DFlags VisualFlags { get; private set; } = Visual3DFlags.BBoxDirty;

        internal bool IsVisualVisible { get; set; } = true;

        public virtual int VisualChildrenCount => 0;

        private Vector2 _visualOffset;
        private RenderData _renderData;
        internal readonly IRenderableObject _renderableObject;

        private Visual3D _parent;
        internal Visual3D Parent
        {
            get { return _parent; }
            private set
            {
                if (_parent != value)
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

        public Visual3D()
        {
            _renderableObject = DeviceContext.Current.CreateRenderableObject();
            InvalidateRender();
            //InvalidateBoundingBox();
        }


        internal void Render()
        {
            ClearFlags(Visual3DFlags.RenderDirty);
            using (var drawingContext = RenderOpen())
            {
                OnRender(drawingContext);
            }
        }

        internal void InvalidateRender()
        {
            SetUIFlags(Visual3DFlags.RenderDirty);
            LayoutManager.Current.RegisterRender(this);
        }

        private void SetUIFlags(Visual3DFlags flags)
        {
            VisualFlags |= flags;
        }

        private void ClearFlags(Visual3DFlags flags)
        {
            VisualFlags &= ~flags;
        }

        private IDrawingContext3D RenderOpen()
        {
            return new Visual3DDrawingContext(this);
        }

        private void RenderClose(RenderData renderData)
        {
            _renderData = renderData;
            _renderableObject.SetContent(_renderData.Close());
        }

        protected virtual void OnRender(IDrawingContext3D drawingContext)
        {
            drawingContext.DrawGeometry3D(new BoxGeometry3D
            {
                Position = new Point3D(),
                Width = 200,
                Height = 100,
                Depth = 50
            }, null, Color.FromArgb(0xFFFF0000));
        }

        internal virtual void RenderContent(ref SwapChainDrawingContext context)
        {
            _renderableObject.Render();
        }

        protected virtual void OnParentChanged(Visual3D visual)
        {
            ParentChanged?.Invoke(this, EventArgs.Empty);
        }

        private void UpdateVisualOffset(Vector2 value)
        {
            _renderableObject.SetOffset(value.X, value.Y);
            //foreach (var child in VisualChildren)
            //    child.UpdateVisualOffset(child.VisualOffset);
        }

        public virtual Visual3D GetVisualChildAt(int index)
        {
            throw new ArgumentOutOfRangeException(nameof(index));
        }

        public IEnumerable<Visual3D> VisualChildren
        {
            get
            {
                var count = VisualChildrenCount;
                for (int i = 0; i < count; i++)
                    yield return GetVisualChildAt(i);
            }
        }

        class Visual3DDrawingContext : RenderDataDrawingContext
        {
            private readonly Visual3D _visual;
            public Visual3DDrawingContext(Visual3D visual)
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
}
