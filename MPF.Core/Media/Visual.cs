using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class Visual : DependencyObject
    {
        public static readonly DependencyProperty<bool> IsHitTestVisibleProperty = DependencyProperty.Register(nameof(IsHitTestVisible),
            typeof(Visual), true);

        private static readonly DependencyProperty<Geometry> VisualClipProperty = DependencyProperty.Register(nameof(VisualClip),
            typeof(Visual), Geometry.Empty);

        public bool IsHitTestVisible
        {
            get { return GetValue(IsHitTestVisibleProperty); }
            set { SetValue(IsHitTestVisibleProperty, value); }
        }

        private Geometry VisualClip
        {
            get { return GetValue(VisualClipProperty); }
            set { SetValue(VisualClipProperty, value); }
        }

        public Visual()
        {

        }

        public void HitTest(PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            if (!IsHitTestVisible) return;

        }

        protected virtual PointHitTestResult HitTestOverride(PointHitTestParameters param)
        {
            return null;
        }
    }

    public enum HitTestFilterBehavior
    {
        ContinueSkipChildren,
        ContinueSkipSelfAndChildren,
        ContinueSkipSelf,
        Continue,
        Stop
    }

    public enum HitTestResultBehavior
    {
        Stop,
        Continue
    }

    public delegate HitTestFilterBehavior HitTestFilterCallback<in T>(T obj);
    public delegate HitTestResultBehavior HitTestResultCallback<in T>(T result);
}
