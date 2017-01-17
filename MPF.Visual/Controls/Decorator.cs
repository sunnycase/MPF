using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MPF.Media;
using System.Collections;

namespace MPF.Controls
{
    public class Decorator : FrameworkElement
    {
        private UIElement _child;
        public UIElement Child
        {
            get { return _child; }
            set
            {
                if (_child != value)
                {
                    if (_child != null)
                        RemoveVisualChild(_child);
                    _child = value;
                    if (_child != null)
                        AddVisualChild(_child);
                    InvalidateMeasure();
                }
            }
        }

        public override int VisualChildrenCount => _child == null ? 0 : 1;

        protected override IEnumerator LogicalChildren
        {
            get
            {
                if (_child != null)
                    yield return _child;
            }
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            return finalSize;
        }

        protected override void OnAfterArrange()
        {
            _child?.InvalidateArrange();
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            if (_child != null)
            {
                _child.Measure(availableSize);
                return _child.DesiredSize;
            }
            return default(Size);
        }

        public override Visual GetVisualChildAt(int index)
        {
            if (_child == null || index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            return _child;
        }
    }
}
