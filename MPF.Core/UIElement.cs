using MPF.Data;
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
        ArrangeDirty = 0x4,
        Visible = 0x8
    }

    public class UIElement : Visual
    {
        private UIElementFlags _uiFlags = UIElementFlags.ArrangeDirty | UIElementFlags.Visible;
        internal UIElementFlags UIFlags => _uiFlags;
        private Size _renderSize;

        protected internal virtual IEnumerable<UIElement> LogicalChildren
        {
            get { yield break; }
        }

        public Size RenderSize
        {
            get { return _renderSize; }
            set { _renderSize = value; }
        }

        public static readonly DependencyProperty<Visibility> VisibilityProperty = DependencyProperty.Register(nameof(Visibility), typeof(UIElement),
            new PropertyMetadata<Visibility>(Visibility.Visible, OnVisibilityPropertyChanged));

        public Visibility Visibility
        {
            get { return GetValue(VisibilityProperty); }
            set { this.SetLocalValue(VisibilityProperty, value); }
        }

        private Size _desiredSize;
        public Size DesiredSize => Visibility == Visibility.Collapsed ? default(Size) : _desiredSize;

        public UIElement()
        {
            UpdateVisibilityFlag(Visibility);
        }

        internal new void Render()
        {
            ClearFlags(UIElementFlags.RenderDirty);
            base.Render();
        }

        public void InvalidateArrange()
        {
            SetUIFlags(UIElementFlags.ArrangeDirty);
        }

        public void Arrange(Rect finalRect)
        {
            ClearFlags(UIElementFlags.ArrangeDirty);
            ArrangeOverride(finalRect);
            InvalidateRender();
        }

        protected virtual void ArrangeOverride(Rect finalRect)
        {
            RenderSize = finalRect.Size;
            VisualOffset = (Vector2)finalRect.Location;
        }

        public void InvalidateMeasure()
        {
            SetUIFlags(UIElementFlags.MeasureDirty);
        }

        public void Measure(Size availableSize)
        {
            ClearFlags(UIElementFlags.MeasureDirty);
            _desiredSize = MeasureOverride(availableSize);
            InvalidateArrange();
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

        internal void InvalidateRender()
        {
            SetUIFlags(UIElementFlags.RenderDirty);
        }

        private void UpdateVisibilityFlag(Visibility value)
        {
            if (value == Visibility.Visible)
                SetUIFlags(UIElementFlags.Visible);
            else
                ClearFlags(UIElementFlags.Visible);
        }

        private static void OnVisibilityPropertyChanged(object sender, PropertyChangedEventArgs<Visibility> e)
        {
            ((UIElement)sender).UpdateVisibilityFlag(e.NewValue);
        }
    }
}
