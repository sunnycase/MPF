using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    [Flags]
    internal enum UIElementFlags : uint
    {
        None = 0x0,
        MeasureDirty = 0x1,
        RenderDirty = 0x2,
        ArrangeDirty = 0x4
    }

    public class UIElement : Visual
    {
        private UIElementFlags _uiFlags = (UIElementFlags)~0u;
        internal UIElementFlags UIFlags => _uiFlags;
        private Size _renderSize;

        protected internal virtual IEnumerable<UIElement> LogicalChildren
        {
            get { return null; }
        }

        public Size RenderSize
        {
            get { return _renderSize; }
            set { _renderSize = value; }
        }

        public UIElement()
        {

        }

        internal new void Render()
        {
            _renderRegistered = false;
            ClearFlags(UIElementFlags.RenderDirty);
            base.Render();
        }

        public void Arrange(Rect finalRect)
        {
            ClearFlags(UIElementFlags.ArrangeDirty);
            ArrangeOverride(finalRect);
        }

        protected virtual void ArrangeOverride(Rect finalRect)
        {
            RenderSize = finalRect.Size;
            VisualOffset = (Vector2)finalRect.Location;
        }

        bool _measureRegistered = false;
        public void InvalidateMeasure()
        {
            if (!_measureRegistered)
            {
                _measureRegistered = true;
                SetUIFlags(UIElementFlags.MeasureDirty);
                SetUIFlags(UIElementFlags.ArrangeDirty);
            }
        }

        public void Measure(Size availableSize)
        {
            _measureRegistered = false;
            ClearFlags(UIElementFlags.MeasureDirty);
            MeasureOverride(availableSize);
        }

        protected virtual Size MeasureOverride(Size availableSize)
        {
            return Size.Zero;
        }

        private void SetUIFlags(UIElementFlags flags)
        {
            _uiFlags |= flags;
        }

        private void ClearFlags(UIElementFlags flags)
        {
            _uiFlags &= ~flags;
        }


        bool _renderRegistered = false;
        internal void RegisterRender()
        {
            if (!_renderRegistered)
            {
                _renderRegistered = true;
                SetUIFlags(UIElementFlags.RenderDirty);
            }
        }
    }
}
