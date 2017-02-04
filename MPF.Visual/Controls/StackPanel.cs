using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class StackPanel : Panel
    {
        public static readonly DependencyProperty<Orientation> OrientationProperty = DependencyProperty.Register(nameof(Orientation),
            typeof(StackPanel), new FrameworkPropertyMetadata<Orientation>(Orientation.Vertical, uiOptions: UIPropertyMetadataOptions.AffectMeasure));

        public Orientation Orientation
        {
            get { return GetValue(OrientationProperty); }
            set { this.SetLocalValue(OrientationProperty, value); }
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            var size = default(Size);
            var isHorizontal = Orientation == Orientation.Horizontal;
            if (isHorizontal)
            {
                availableSize.Width = float.PositiveInfinity;
            }
            else
            {
                availableSize.Height = float.PositiveInfinity;
            }
            foreach (var child in Children)
            {
                child.Measure(availableSize);
                var desiredSize = child.DesiredSize;
                if (isHorizontal)
                {
                    size.Width += desiredSize.Width;
                    size.Height = Math.Max(size.Height, desiredSize.Height);
                }
                else
                {
                    size.Width = Math.Max(size.Width, desiredSize.Width);
                    size.Height += desiredSize.Height;
                }
            }
            return size;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            var finalRect = new Rect(Point.Zero, finalSize);
            var isHorizontal = Orientation == Orientation.Horizontal;

            float offset = 0.0f;
            foreach (var child in Children)
            {
                if(isHorizontal)
                {
                    finalRect.Left += offset;
                    offset = child.DesiredSize.Width;
                    finalRect.Width = offset;
                    finalRect.Height = Math.Max(finalSize.Height, child.DesiredSize.Height);
                }
                else
                {
                    finalRect.Top += offset;
                    offset = child.DesiredSize.Height;
                    finalRect.Width = Math.Max(finalSize.Width, child.DesiredSize.Width);
                    finalRect.Height = offset;
                }
                child.Arrange(finalRect);
            }
            return finalSize;
        }
    }
}
