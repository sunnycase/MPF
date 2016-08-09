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

        public static readonly DependencyProperty<Visibility> VisibilityProperty = DependencyProperty.Register(nameof(Visibility), typeof(UIElement),
            new PropertyMetadata<Visibility>(Visibility.Visible, OnVisibilityPropertyChanged));

        public Visibility Visibility
        {
            get { return GetValue(VisibilityProperty); }
            set { SetValue(VisibilityProperty, value); }
        }

        private Size _desiredSize;
        public Size DesiredSize => Visibility == Visibility.Collapsed ? default(Size) : _desiredSize;

        public UIElement()
        {
            UpdateVisibilityFlag(Visibility);
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
            _desiredSize = MeasureOverride(availableSize);
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
