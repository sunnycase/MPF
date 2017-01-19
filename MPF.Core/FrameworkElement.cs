using MPF.Data;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    public class FrameworkElement : UIElement
    {
        public static readonly DependencyProperty<Thickness> MarginProperty = DependencyProperty.Register(nameof(Margin),
            typeof(FrameworkElement), new UIPropertyMetadata<Thickness>(default(Thickness), UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<HorizontalAlignment> HorizontalAlignmentProperty = DependencyProperty.Register(nameof(HorizontalAlignment),
            typeof(FrameworkElement), new UIPropertyMetadata<HorizontalAlignment>(HorizontalAlignment.Stretch, UIPropertyMetadataOptions.AffectArrange));

        public static readonly DependencyProperty<VerticalAlignment> VerticalAlignmentProperty = DependencyProperty.Register(nameof(VerticalAlignment),
            typeof(FrameworkElement), new UIPropertyMetadata<VerticalAlignment>(VerticalAlignment.Stretch, UIPropertyMetadataOptions.AffectArrange));

        public static readonly DependencyProperty<object> DataContextProperty = DependencyProperty.Register(nameof(DataContext),
            typeof(FrameworkElement), new FrameworkPropertyMetadata<object>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.None));

        public static readonly DependencyProperty<Style> StyleProperty = DependencyProperty.Register(nameof(Style),
            typeof(FrameworkElement), new PropertyMetadata<Style>(DependencyProperty.UnsetValue, OnStylePropertyChanged));

        public HorizontalAlignment HorizontalAlignment
        {
            get { return GetValue(HorizontalAlignmentProperty); }
            set { this.SetLocalValue(HorizontalAlignmentProperty, value); }
        }

        public VerticalAlignment VerticalAlignment
        {
            get { return GetValue(VerticalAlignmentProperty); }
            set { this.SetLocalValue(VerticalAlignmentProperty, value); }
        }

        public Thickness Margin
        {
            get { return GetValue(MarginProperty); }
            set { this.SetLocalValue(MarginProperty, value); }
        }

        public object DataContext
        {
            get { return GetValue(DataContextProperty); }
            set { this.SetLocalValue(DataContextProperty, value); }
        }

        public Style Style
        {
            get { return GetValue(StyleProperty); }
            set { this.SetLocalValue(StyleProperty, value); }
        }

        protected internal virtual IEnumerator LogicalChildren
        {
            get { yield break; }
        }

        protected sealed override void ArrangeOverride(Rect finalRect)
        {
            var size = finalRect.Size;
            var margin = Margin;

            size = new Size(size.Width - margin.Left - margin.Right, size.Height - margin.Top - margin.Bottom);
            var renderSize = ArrangeOverride(ComputeAlignmentSize(size));
            RenderSize = renderSize;
            var offset = ComputeAlignmentOffset(size);
            offset.X += finalRect.Left + margin.Left;
            offset.Y += finalRect.Top + margin.Top;
            VisualOffset = offset;
            OnAfterArrange();
        }

        protected virtual void OnAfterArrange()
        {

        }

        protected virtual Size ArrangeOverride(Size finalSize)
        {
            return finalSize;
        }

        private Size ComputeAlignmentSize(Size clientSize)
        {
            float width = 0;
            switch (HorizontalAlignment)
            {
                case HorizontalAlignment.Left:
                case HorizontalAlignment.Center:
                case HorizontalAlignment.Right:
                    width = Math.Min(clientSize.Width, DesiredSize.Width);
                    break;
                case HorizontalAlignment.Stretch:
                    width = clientSize.Width;
                    break;
            }

            float height = 0;
            switch (VerticalAlignment)
            {
                case VerticalAlignment.Top:
                case VerticalAlignment.Center:
                case VerticalAlignment.Bottom:
                    height = Math.Min(clientSize.Height, DesiredSize.Height);
                    break;
                case VerticalAlignment.Stretch:
                    height = clientSize.Height;
                    break;
            }
            return new Size(width, height);
        }

        private Vector2 ComputeAlignmentOffset(Size clientSize)
        {
            float left = 0;
            switch (HorizontalAlignment)
            {
                case HorizontalAlignment.Left:
                case HorizontalAlignment.Stretch:
                    left = 0;
                    break;
                case HorizontalAlignment.Center:
                    left = (clientSize.Width - RenderSize.Width) / 2;
                    break;
                case HorizontalAlignment.Right:
                    left = clientSize.Width - RenderSize.Width;
                    break;
            }

            float top = 0;
            switch (VerticalAlignment)
            {
                case VerticalAlignment.Top:
                case VerticalAlignment.Stretch:
                    top = 0;
                    break;
                case VerticalAlignment.Center:
                    top = (clientSize.Height - RenderSize.Height) / 2;
                    break;
                case VerticalAlignment.Bottom:
                    top = clientSize.Height - RenderSize.Height;
                    break;
            }

            return new Vector2(left, top);
        }

        private static void OnStylePropertyChanged(object sender, PropertyChangedEventArgs<Style> e)
        {
            (sender as FrameworkElement)?.OnStyleChanged(e.NewValue);
        }

        private void OnStyleChanged(Style style)
        {
            if (style != null)
                style.Apply(this);
            else
                Style.Clear(this);
        }
    }
}
