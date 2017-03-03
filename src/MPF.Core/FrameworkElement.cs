using MPF.Data;
using MPF.Media;
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

        public static readonly DependencyProperty<float> WidthProperty = DependencyProperty.Register(nameof(Width),
            typeof(FrameworkElement), new UIPropertyMetadata<float>(float.NaN, UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<float> HeightProperty = DependencyProperty.Register(nameof(Height),
            typeof(FrameworkElement), new UIPropertyMetadata<float>(float.NaN, UIPropertyMetadataOptions.AffectMeasure));

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

        public float Width
        {
            get { return GetValue(WidthProperty); }
            set { this.SetLocalValue(WidthProperty, value); }
        }

        public float Height
        {
            get { return GetValue(HeightProperty); }
            set { this.SetLocalValue(HeightProperty, value); }
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

        public FrameworkElement()
        {
            LayoutManager.Current.RegisterInitialize(this);
        }

        protected sealed override void ArrangeCore(Rect finalRect)
        {
            var size = finalRect.Size;
            if (HorizontalAlignment != HorizontalAlignment.Stretch)
                size.Width = Math.Min(size.Width, DesiredSize.Width);
            if (VerticalAlignment != VerticalAlignment.Stretch)
                size.Height = Math.Min(size.Height, DesiredSize.Height);

            var margin = Margin;
            size.Width -= margin.Left + margin.Right;
            size.Height -= margin.Top + margin.Bottom;

            var minMax = GetMinMax();
            float maxWidth = Math.Max(DesiredSize.Width, minMax.MaxWidth);
            size.Width = Math.Min(size.Width, maxWidth);
            float maxHeight = Math.Max(DesiredSize.Height, minMax.MaxHeight);
            size.Height = Math.Min(size.Height, maxHeight);

            var renderSize = ArrangeOverride(size);
            RenderSize = renderSize;

            size = new Size(Math.Max(finalRect.Width - margin.Left - margin.Right, 0),
                Math.Max(finalRect.Height - margin.Top - margin.Bottom, 0));
            var offset = ComputeAlignmentOffset(size);
            offset.X += finalRect.Left + margin.Left;
            offset.Y += finalRect.Top + margin.Top;
            VisualOffset = offset;
        }

        protected virtual Size ArrangeOverride(Size finalSize)
        {
            return finalSize;
        }

        protected sealed override Size MeasureCore(Size availableSize)
        {
            var margin = Margin;
            var size = new Size(Math.Max(availableSize.Width - margin.Left - margin.Right, 0), 
                Math.Max(availableSize.Height - margin.Top - margin.Bottom, 0));
            var minMax = GetMinMax();
            size.Width = Math.Max(minMax.MinWidth, Math.Min(size.Width, minMax.MaxWidth));
            size.Height = Math.Max(minMax.MinHeight, Math.Min(size.Height, minMax.MaxHeight));
            size = MeasureOverride(size);
            size = new Size(Math.Max(size.Width, minMax.MinWidth), Math.Max(size.Height, minMax.MinHeight));

            float width = size.Width + margin.Left + margin.Right;
            float height = size.Height + margin.Top + margin.Bottom;
            return new Size(Math.Max(0, width), Math.Max(0, height));
        }

        private MinMax GetMinMax()
        {
            var width = Width;
            var height = Height;
            return new MinMax
            {
                MinWidth = float.IsNaN(width) ? 0 : width,
                MaxWidth = float.IsNaN(width) ? float.PositiveInfinity : width,
                MinHeight = float.IsNaN(height) ? 0 : height,
                MaxHeight = float.IsNaN(height) ? float.PositiveInfinity : height
            };
        }

        struct MinMax
        {
            public float MinWidth;
            public float MaxWidth;
            public float MinHeight;
            public float MaxHeight;
        }

        protected virtual Size MeasureOverride(Size availableSize)
        {
            return default(Size);
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

        bool _styleLoaded = false;

        private void OnStyleChanged(Style style)
        {
            _styleLoaded = true;
            if (style != null)
                style.Apply(this);
            else
                Style.Clear(this);
        }

        internal void OnInitialize()
        {
            if(!_styleLoaded)
            {
                LoadStyle();
                _styleLoaded = true;
            }
            OnInitializeOverride();
        }

        protected virtual void OnInitializeOverride()
        {

        }

        private void LoadStyle()
        {
            var style = Style;
            style?.Apply(this);
        }
    }
}
