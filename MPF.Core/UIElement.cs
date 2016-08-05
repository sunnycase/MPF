using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public class UIElement : Visual
    {
        public UIElement()
        {
            _renderableObject.SetMeasureCallback(OnMeasure);
        }

        bool _measureRegistered = false;
        internal void RegisterMeasure()
        {
            if (!_measureRegistered)
            {
                _measureRegistered = true;
                _renderableObject.SetFlags(RenderableObjectFlags.MeasureDirty);
            }
        }

        internal virtual void OnMeasure()
        {
            _measureRegistered = false;
            Measure(Size.Zero);
        }

        public void Measure(Size availableSize)
        {
            MeasureOverride(availableSize);
        }

        protected virtual Size MeasureOverride(Size availableSize)
        {
            return Size.Zero;
        }
    }
}
