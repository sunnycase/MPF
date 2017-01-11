using MPF.Data;
using System;
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

        public static readonly DependencyProperty<object> DataContextProperty = DependencyProperty.Register(nameof(DataContext),
            typeof(FrameworkElement), new FrameworkPropertyMetadata<object>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.None));

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

        protected sealed override void ArrangeOverride(Rect finalRect)
        {
            var size = finalRect.Size;
            var margin = Margin;
            size.Width = Math.Max(0.0f, size.Width - margin.Left - margin.Right);
            size.Height = Math.Max(0.0f, size.Height - margin.Top - margin.Bottom);

            var renderSize = ArrangeOverride(size);
            RenderSize = renderSize;
            var offset = ComputeAlignmentOffset(renderSize);
            offset.X += finalRect.X + margin.Left;
            offset.Y += finalRect.Y + margin.Top;
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

        private Vector2 ComputeAlignmentOffset(Size clientSize)
        {
            return Vector2.Zero;
        }
    }
}
