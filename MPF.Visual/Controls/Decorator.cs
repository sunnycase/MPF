using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

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
                    _child = value;
                    InvalidateMeasure();
                }
            }
        }

        protected override IEnumerable<UIElement> LogicalChildren
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
    }
}
